/*
 * main.c -- start the ball rolling
 */


#include "stdarg.h"
#include "start.h"


/**************************************************************/


int currentTask;
unsigned int *currentStkTop;


/**************************************************************/


unsigned int task0Stack[256];
unsigned int *task0StkTop = task0Stack + 256;


unsigned char task1Code[] = {
  #include "task1.dump"
};

unsigned int task1Stack[256];
unsigned int *task1StkTop = task1Stack + 256;


unsigned char task2Code[] = {
  #include "task2.dump"
};

unsigned int task2Stack[256];
unsigned int *task2StkTop = task2Stack + 256;


/**************************************************************/


void putchar(char c) {
  unsigned int *base;

  if (c == '\n') {
    putchar('\r');
  }
  base = (unsigned int *) 0xF0300000;
  while ((*(base + 2) & 1) == 0) ;
  *(base + 3) = c;
}


void puts(char *s) {
  char c;

  while ((c = *s++) != '\0') {
    putchar(c);
  }
}


void printn(int n) {
  int a;

  if (n < 0) {
    putchar('-');
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    printn(a);
  }
  putchar(n % 10 + '0');
}


void printu(unsigned int n, unsigned int b) {
  unsigned int a;

  a = n / b;
  if (a != 0) {
    printu(a, b);
  }
  putchar("0123456789ABCDEF"[n % b]);
}


void printf(char *fmt, ...) {
  va_list ap;
  char c;
  int n;
  unsigned int u;
  char *s;

  va_start(ap, fmt);
  while (1) {
    while ((c = *fmt++) != '%') {
      if (c == '\0') {
        va_end(ap);
        return;
      }
      putchar(c);
    }
    c = *fmt++;
    if (c == 'd') {
      n = va_arg(ap, int);
      printn(n);
    } else
    if (c == 'u' || c == 'o' || c == 'x') {
      u = va_arg(ap, int);
      printu(u, c == 'o' ? 8 : (c == 'x' ? 16 : 10));
    } else
    if (c == 's') {
      s = va_arg(ap, char *);
      puts(s);
    } else {
      putchar(c);
    }
  }
}


/**************************************************************/


static char *exceptionCause[32] = {
  /* 00 */  "terminal 0 transmitter interrupt",
  /* 01 */  "terminal 0 receiver interrupt",
  /* 02 */  "terminal 1 transmitter interrupt",
  /* 03 */  "terminal 1 receiver interrupt",
  /* 04 */  "keyboard interrupt",
  /* 05 */  "mouse interrupt",
  /* 06 */  "unknown interrupt",
  /* 07 */  "unknown interrupt",
  /* 08 */  "disk interrupt",
  /* 09 */  "unknown interrupt",
  /* 10 */  "unknown interrupt",
  /* 11 */  "unknown interrupt",
  /* 12 */  "unknown interrupt",
  /* 13 */  "unknown interrupt",
  /* 14 */  "timer 0 interrupt",
  /* 15 */  "timer 1 interrupt",
  /* 16 */  "bus timeout exception",
  /* 17 */  "illegal instruction exception",
  /* 18 */  "privileged instruction exception",
  /* 19 */  "divide instruction exception",
  /* 20 */  "trap instruction exception",
  /* 21 */  "TLB miss exception",
  /* 22 */  "TLB write exception",
  /* 23 */  "TLB invalid exception",
  /* 24 */  "illegal address exception",
  /* 25 */  "privileged address exception",
  /* 26 */  "unknown exception",
  /* 27 */  "unknown exception",
  /* 28 */  "unknown exception",
  /* 29 */  "unknown exception",
  /* 30 */  "unknown exception",
  /* 31 */  "unknown exception"
};


void defaultISR(int irq, unsigned int *registers) {
  printf("\n%s\n", exceptionCause[irq]);
}


void initInterrupts(void) {
  int i;

  for (i = 0; i < 32; i++) {
    setISR(i, defaultISR);
  }
}


/**************************************************************/


unsigned int getNumber(unsigned char *p) {
  return (unsigned int) *(p + 0) << 24 |
         (unsigned int) *(p + 1) << 16 |
         (unsigned int) *(p + 2) <<  8 |
         (unsigned int) *(p + 3) <<  0;
}


void loadTask(unsigned char *exec,
              unsigned int physCodeAddr,
              unsigned int physDataAddr) {
  unsigned int magic;
  unsigned int nsegs;
  unsigned int osegs;
  unsigned int odata;
  unsigned int coffs;
  unsigned int caddr;
  unsigned int csize;
  unsigned int doffs;
  unsigned int daddr;
  unsigned int dsize;
  unsigned int boffs;
  unsigned int baddr;
  unsigned int bsize;
  unsigned char *code;
  unsigned char *data;
  unsigned char *virtLoadAddr;
  int i;

  magic = getNumber(exec + 0 * 4);
  nsegs = getNumber(exec + 2 * 4);
  if (magic != 0x8F0B45C0 || nsegs != 3) {
    printf("Error: Load module is not executable!\n");
    while (1) ;
  }
  osegs = getNumber(exec + 1 * 4);
  odata = getNumber(exec + 7 * 4);
  printf("osegs = 0x%x, odata = 0x%x\n", osegs, odata);
  coffs = getNumber(exec + osegs + 0 * (5 * 4) + 1 * 4);
  caddr = getNumber(exec + osegs + 0 * (5 * 4) + 2 * 4);
  csize = getNumber(exec + osegs + 0 * (5 * 4) + 3 * 4);
  printf("coffs = 0x%x, caddr = 0x%x, csize = 0x%x\n",
         coffs, caddr, csize);
  doffs = getNumber(exec + osegs + 1 * (5 * 4) + 1 * 4);
  daddr = getNumber(exec + osegs + 1 * (5 * 4) + 2 * 4);
  dsize = getNumber(exec + osegs + 1 * (5 * 4) + 3 * 4);
  printf("doffs = 0x%x, daddr = 0x%x, dsize = 0x%x\n",
         doffs, daddr, dsize);
  boffs = getNumber(exec + osegs + 2 * (5 * 4) + 1 * 4);
  baddr = getNumber(exec + osegs + 2 * (5 * 4) + 2 * 4);
  bsize = getNumber(exec + osegs + 2 * (5 * 4) + 3 * 4);
  printf("boffs = 0x%x, baddr = 0x%x, bsize = 0x%x\n",
         boffs, baddr, bsize);
  code = exec + odata + coffs;
  data = exec + odata + doffs;
  virtLoadAddr = (unsigned char *) (0xC0000000 | physCodeAddr);
  for (i = 0; i < csize; i++) {
    *virtLoadAddr++ = *code++;
  }
  virtLoadAddr = (unsigned char *) (0xC0000000 | physDataAddr);
  for (i = 0; i < dsize; i++) {
    *virtLoadAddr++ = *data++;
  }
  for (i = 0; i < bsize; i++) {
    *virtLoadAddr++ = '\0';
  }
  syncCaches();
}


/**************************************************************/


void trapISR(int irq, unsigned int *registers) {
  /* 'putchar' is the only system call yet */
  putchar(registers[4]);
  /* skip the trap instruction */
  registers[30] += 4;
}


/**************************************************************/


#define T1C_FRM		64		/* code at 256 k */
#define T1D_FRM		68		/* data at 272 k */
#define T1S_FRM		80		/* user-mode stack at 320 k */

#define T2C_FRM		96		/* code at 256 k */
#define T2D_FRM		100		/* data at 272 k */
#define T2S_FRM		112		/* user-mode stack at 320 k */


void missISR(int irq, unsigned int *registers) {
  if (currentTask == 1) {
    setTLB(5, 0x00000000, T1C_FRM << 12 | 0x01);	/* code */
    setTLB(27, 0x00001000, T1D_FRM << 12 | 0x03);	/* data */
    setTLB(22, 0x7FFFF000, T1S_FRM << 12 | 0x03);	/* stack */
  } else if (currentTask == 2) {
    setTLB(5, 0x00000000, T2C_FRM << 12 | 0x01);	/* code */
    setTLB(27, 0x00001000, T2D_FRM << 12 | 0x03);	/* data */
    setTLB(22, 0x7FFFF000, T2S_FRM << 12 | 0x03);	/* stack */
  }
}


void flushTLB(void) {
  unsigned int invalPage;
  int i;

  invalPage = 0xC0000000;
  for (i = 0; i < 32; i++) {
    setTLB(i, invalPage, 0);
    invalPage += (1 << 12);
  }
}


/**************************************************************/


void initTimer(void) {
  unsigned int *timerBase;

  timerBase = (unsigned int *) 0xF0000000;
  *(timerBase + 1) = 50000000;
  *timerBase = 2;
  orMask(1 << 14);
}


int task1Started = 0;
int task2Started = 0;


void timerISR(int irq, unsigned int *registers) {
  unsigned int *timerBase;
  unsigned int dummy;

  timerBase = (unsigned int *) 0xF0000000;
  dummy = *timerBase;
  printf(">|<");
  if (currentTask == 0) {
    if (!task1Started) {
      /* load & start task1 */
      printf("\nOS: loading task1\n");
      loadTask(task1Code, T1C_FRM << 12, T1D_FRM << 12);
      printf("OS: starting task1\n");
      task1Started = 1;
      currentTask = 1;
      currentStkTop = task1StkTop;
      flushTLB();
      startTask();
    }
  } else if (currentTask == 1) {
    if (!task2Started) {
      /* load & start task2 */
      printf("\nOS: loading task2\n");
      loadTask(task2Code, T2C_FRM << 12, T2D_FRM << 12);
      printf("OS: starting task2\n");
      task2Started = 1;
      currentTask = 2;
      currentStkTop = task2StkTop;
      flushTLB();
      startTask();
    } else {
      /* switch tasks */
      currentTask = 2;
      currentStkTop = task2StkTop;
      flushTLB();
    }
  } else if (currentTask == 2) {
    /* switch tasks */
    currentTask = 1;
    currentStkTop = task1StkTop;
    flushTLB();
  }
}


/**************************************************************/


void main(void) {
  currentTask = 0;
  currentStkTop = task0StkTop;
  printf("\n");
  printf("OS: initializing interrupts\n");
  initInterrupts();
  setISR(20, trapISR);
  setISR(21, missISR);
  setISR(14, timerISR);
  printf("OS: initializing timer\n");
  initTimer();
  printf("OS: waiting for interrupt...\n");
  enable();
  while (1) ;
}
