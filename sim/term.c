/*
 * term.c -- terminal simulation
 */


#ifdef __linux__
#define _XOPEN_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "cpu.h"
#include "timer.h"
#include "term.h"


/**************************************************************/


static Bool debug = false;


typedef struct {
  pid_t pid;
  FILE *in;
  FILE *out;
  Word rcvrCtrl;
  Word rcvrData;
  int rcvrIRQ;
  Word xmtrCtrl;
  Word xmtrData;
  int xmtrIRQ;
} Terminal;


static Terminal terminals[MAX_NTERMS];
static int numTerminals;


/**************************************************************/


static void rcvrCallback(int dev) {
  int c;

  if (debug) {
    cPrintf("\n**** TERM RCVR CALLBACK ****\n");
  }
  timerStart(TERM_RCVR_USEC, rcvrCallback, dev);
  c = fgetc(terminals[dev].in);
  if (c == EOF) {
    /* no character typed */
    return;
  }
  /* any character typed */
  terminals[dev].rcvrData = c & 0xFF;
  terminals[dev].rcvrCtrl |= TERM_RCVR_RDY;
  if (terminals[dev].rcvrCtrl & TERM_RCVR_IEN) {
    /* raise terminal rcvr interrupt */
    cpuSetInterrupt(terminals[dev].rcvrIRQ);
  }
}


static void xmtrCallback(int dev) {
  if (debug) {
    cPrintf("\n**** TERM XMTR CALLBACK ****\n");
  }
  fputc(terminals[dev].xmtrData & 0xFF, terminals[dev].out);
  terminals[dev].xmtrCtrl |= TERM_XMTR_RDY;
  if (terminals[dev].xmtrCtrl & TERM_XMTR_IEN) {
    /* raise terminal xmtr interrupt */
    cpuSetInterrupt(terminals[dev].xmtrIRQ);
  }
}


/**************************************************************/


Word termRead(Word addr) {
  int dev, reg;
  Word data;

  if (debug) {
    cPrintf("\n**** TERM READ from 0x%08X", addr);
  }
  dev = addr >> 12;
  if (dev >= numTerminals) {
    /* illegal device */
    throwException(EXC_BUS_TIMEOUT);
  }
  reg = addr & 0x0FFF;
  if (reg == TERM_RCVR_CTRL) {
    data = terminals[dev].rcvrCtrl;
  } else
  if (reg == TERM_RCVR_DATA) {
    terminals[dev].rcvrCtrl &= ~TERM_RCVR_RDY;
    if (terminals[dev].rcvrCtrl & TERM_RCVR_IEN) {
      /* lower terminal rcvr interrupt */
      cpuResetInterrupt(terminals[dev].rcvrIRQ);
    }
    data = terminals[dev].rcvrData;
  } else
  if (reg == TERM_XMTR_CTRL) {
    data = terminals[dev].xmtrCtrl;
  } else
  if (reg == TERM_XMTR_DATA) {
    /* this register is write-only */
    throwException(EXC_BUS_TIMEOUT);
  } else {
    /* illegal register */
    throwException(EXC_BUS_TIMEOUT);
  }
  if (debug) {
    cPrintf(", data = 0x%08X ****\n", data);
  }
  return data;
}


void termWrite(Word addr, Word data) {
  int dev, reg;

  if (debug) {
    cPrintf("\n**** TERM WRITE to 0x%08X, data = 0x%08X ****\n",
            addr, data);
  }
  dev = addr >> 12;
  if (dev >= numTerminals) {
    /* illegal device */
    throwException(EXC_BUS_TIMEOUT);
  }
  reg = addr & 0x0FFF;
  if (reg == TERM_RCVR_CTRL) {
    if (data & TERM_RCVR_IEN) {
      terminals[dev].rcvrCtrl |= TERM_RCVR_IEN;
    } else {
      terminals[dev].rcvrCtrl &= ~TERM_RCVR_IEN;
    }
    if (data & TERM_RCVR_RDY) {
      terminals[dev].rcvrCtrl |= TERM_RCVR_RDY;
    } else {
      terminals[dev].rcvrCtrl &= ~TERM_RCVR_RDY;
    }
    if ((terminals[dev].rcvrCtrl & TERM_RCVR_IEN) != 0 &&
        (terminals[dev].rcvrCtrl & TERM_RCVR_RDY) != 0) {
      /* raise terminal rcvr interrupt */
      cpuSetInterrupt(terminals[dev].rcvrIRQ);
    } else {
      /* lower terminal rcvr interrupt */
      cpuResetInterrupt(terminals[dev].rcvrIRQ);
    }
  } else
  if (reg == TERM_RCVR_DATA) {
    /* this register is read-only */
    throwException(EXC_BUS_TIMEOUT);
  } else
  if (reg == TERM_XMTR_CTRL) {
    if (data & TERM_XMTR_IEN) {
      terminals[dev].xmtrCtrl |= TERM_XMTR_IEN;
    } else {
      terminals[dev].xmtrCtrl &= ~TERM_XMTR_IEN;
    }
    if (data & TERM_XMTR_RDY) {
      terminals[dev].xmtrCtrl |= TERM_XMTR_RDY;
    } else {
      terminals[dev].xmtrCtrl &= ~TERM_XMTR_RDY;
    }
    if ((terminals[dev].xmtrCtrl & TERM_XMTR_IEN) != 0 &&
        (terminals[dev].xmtrCtrl & TERM_XMTR_RDY) != 0) {
      /* raise terminal xmtr interrupt */
      cpuSetInterrupt(terminals[dev].xmtrIRQ);
    } else {
      /* lower terminal xmtr interrupt */
      cpuResetInterrupt(terminals[dev].xmtrIRQ);
    }
  } else
  if (reg == TERM_XMTR_DATA) {
    terminals[dev].xmtrData = data & 0xFF;
    terminals[dev].xmtrCtrl &= ~TERM_XMTR_RDY;
    if (terminals[dev].xmtrCtrl & TERM_XMTR_IEN) {
      /* lower terminal xmtr interrupt */
      cpuResetInterrupt(terminals[dev].xmtrIRQ);
    }
    timerStart(TERM_XMTR_USEC, xmtrCallback, dev);
  } else {
    /* illegal register */
    throwException(EXC_BUS_TIMEOUT);
  }
}


/**************************************************************/


void termReset(void) {
  int i;

  cPrintf("Resetting Terminals...\n");
  for (i = 0; i < numTerminals; i++) {
    terminals[i].rcvrCtrl = 0;
    terminals[i].rcvrData = 0;
    terminals[i].rcvrIRQ = IRQ_TERM_0_RCVR + 2 * i;
    timerStart(TERM_RCVR_USEC, rcvrCallback, i);
    terminals[i].xmtrCtrl = TERM_XMTR_RDY;
    terminals[i].xmtrData = 0;
    terminals[i].xmtrIRQ = IRQ_TERM_0_XMTR + 2 * i;
  }
}


static void makeRaw(int fd) {
  struct termios t;

  tcgetattr(fd, &t);
  t.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
  t.c_oflag &= ~OPOST;
  t.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  t.c_cflag &= ~(CSIZE|PARENB);
  t.c_cflag |= CS8;
  tcsetattr(fd, TCSANOW, &t);
}


void termInit(int numTerms, Bool hasTerm[]) {
  int i;
  int master;
  char slavePath[100];
  int slave;
  char termTitle[100];
  char termSlave[100];

  numTerminals = numTerms;
  for (i = 0; i < numTerminals; i++) {
    /* open pseudo terminal */
    master = open("/dev/ptmx", O_RDWR | O_NONBLOCK);
    if (master < 0) {
      error("cannot open pseudo terminal master for serial line %d", i);
    }
    grantpt(master);
    unlockpt(master);
    strcpy(slavePath, ptsname(master));
    if (debug) {
      cPrintf("pseudo terminal %d: master fd = %d, slave path = '%s'\n",
              i, master, slavePath);
    }
    if (hasTerm[i]) {
      /* connect a terminal to the serial line */
      /* i.e., fork and exec a new xterm process */
      terminals[i].pid = fork();
      if (terminals[i].pid < 0) {
        error("cannot fork xterm process for serial line %d", i);
      }
      if (terminals[i].pid == 0) {
        /* terminal process */
        setpgid(0, 0);
        close(master);
        /* open and configure pseudo terminal slave */
        slave = open(slavePath, O_RDWR | O_NONBLOCK);
        if (slave < 0) {
          error("cannot open pseudo terminal slave '%s'\n", slavePath);
        }
        makeRaw(slave);
        /* exec xterm */
        sprintf(termTitle, "ECO32 Terminal %d", i);
        sprintf(termSlave, "-Sab%d", slave);
        execlp("xterm", "xterm", "-title", termTitle, termSlave, NULL);
        error("cannot exec xterm process for serial line %d", i);
      }
    } else {
      /* leave serial line unconnected */
      terminals[i].pid = 0;
      cPrintf("Serial line %d can be accessed by opening device '%s'.\n",
              i, slavePath);
    }
    fcntl(master, F_SETFL, O_NONBLOCK);
    terminals[i].in = fdopen(master, "r");
    setvbuf(terminals[i].in, NULL, _IONBF, 0);
    terminals[i].out = fdopen(master, "w");
    setvbuf(terminals[i].out, NULL, _IONBF, 0);
    if (hasTerm[i]) {
      /* skip the window id written by xterm */
      while (fgetc(terminals[i].in) != '\n') ;
    }
  }
  termReset();
}


void termExit(void) {
  int i;

  /* kill and wait for all xterm processes */
  for (i = 0; i < numTerminals; i++) {
    if (terminals[i].pid > 0) {
      kill(terminals[i].pid, SIGKILL);
      waitpid(terminals[i].pid, NULL, 0);
    }
  }
}
