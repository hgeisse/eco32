/*
 * multest.c -- test floating point multiplication on the FPGA
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <math.h>
#include <fenv.h>

#include "../../../include/fp.h"


typedef enum { false = 0, true = 1 } Bool;


/**************************************************************/


static int sfd = -1;
static struct termios origOptions;
static struct termios currOptions;


void serialClose(void);


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  serialClose();
  exit(1);
}


/**************************************************************/


void serialOpen(char *serialPort) {
  sfd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);
  if (sfd == -1) {
    error("cannot open serial port '%s'", serialPort);
  }
  tcgetattr(sfd, &origOptions);
  currOptions = origOptions;
  cfsetispeed(&currOptions, B115200);
  cfsetospeed(&currOptions, B115200);
  currOptions.c_cflag |= (CLOCAL | CREAD);
  currOptions.c_cflag &= ~PARENB;
  currOptions.c_cflag &= ~CSTOPB;
  currOptions.c_cflag &= ~CSIZE;
  currOptions.c_cflag |= CS8;
  currOptions.c_cflag &= ~CRTSCTS;
  currOptions.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);
  currOptions.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK);
  currOptions.c_iflag &= ~(INPCK | ISTRIP | INLCR | IGNCR | ICRNL);
  currOptions.c_iflag &= ~(IXON | IXOFF | IXANY);
  currOptions.c_oflag &= ~(OPOST | ONLCR | OCRNL | ONOCR | ONLRET);
  tcsetattr(sfd, TCSANOW, &currOptions);
}


void serialClose(void) {
  if (sfd < 0) {
    return;
  }
  tcsetattr(sfd, TCSANOW, &origOptions);
  close(sfd);
  sfd = -1;
}


int serialSnd(unsigned char b) {
  int n;

  n = write(sfd, &b, 1);
  return n == 1;
}


int serialRcv(unsigned char *bp) {
  int n;

  n = read(sfd, bp, 1);
  return n == 1;
}


/**************************************************************/


unsigned char rcvByte(void) {
  unsigned char b;

  while (!serialRcv(&b)) ;
  return b;
}


unsigned int rcvWord(void) {
  unsigned int w;
  unsigned char b;

  w = 0;
  while (!serialRcv(&b)) ;
  w |= (unsigned int) b <<  0;
  while (!serialRcv(&b)) ;
  w |= (unsigned int) b <<  8;
  while (!serialRcv(&b)) ;
  w |= (unsigned int) b << 16;
  while (!serialRcv(&b)) ;
  w |= (unsigned int) b << 24;
  return w;
}


void sndByte(unsigned char b) {
  while (!serialSnd(b)) ;
}


void sndWord(unsigned int w) {
  while (!serialSnd((w >>  0) & 0xFF)) ;
  while (!serialSnd((w >>  8) & 0xFF)) ;
  while (!serialSnd((w >> 16) & 0xFF)) ;
  while (!serialSnd((w >> 24) & 0xFF)) ;
}


/**************************************************************/


_FP_Word Flags = 0;


_FP_Word fpMul(_FP_Word x, _FP_Word y) {
  _FP_Word z;
  unsigned char f;

  sndWord(x);
  sndWord(y);
  z = rcvWord();
  f = rcvByte();
  Flags |= f;
  return z;
}


/**************************************************************/


_FP_Word Flags_sim = 0;


_FP_Word fpMul_sim(_FP_Word x, _FP_Word y) {
  _FP_Union X, Y, Z;
  fexcept_t flags;

  X.w = x;
  Y.w = y;
  feclearexcept(FE_ALL_EXCEPT);
  fesetround(FE_TONEAREST);
  Z.f = X.f * Y.f;
  fegetexceptflag(&flags, FE_ALL_EXCEPT);
  Flags_sim |= ((flags & FE_DIVBYZERO) ? _FP_I_FLAG : 0) |
               ((flags & FE_INEXACT)   ? _FP_X_FLAG : 0) |
               ((flags & FE_INVALID)   ? _FP_V_FLAG : 0) |
               ((flags & FE_OVERFLOW)  ? _FP_O_FLAG : 0) |
               ((flags & FE_UNDERFLOW) ? _FP_U_FLAG : 0);
  return Z.w;
}


/**************************************************************/


#define LINE_SIZE	200


void usage(char *myself) {
  fprintf(stderr, "usage: %s [-simulate] <serial port>\n", myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  Bool simulate;
  char *serialPort;
  int i;
  char line[LINE_SIZE];
  char *endptr;
  _FP_Word x, y, z;

  simulate = false;
  serialPort = NULL;
  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {
      /* option */
      if (strcmp(argv[i], "-simulate") == 0) {
        simulate = true;
      } else {
        usage(argv[0]);
      }
    } else {
      /* serial port */
      if (serialPort != NULL) {
        usage(argv[0]);
      }
      serialPort = argv[i];
    }
  }
  if (!simulate && serialPort == NULL) {
    error("no serial port specified");
  }
  if (!simulate) {
    serialOpen(serialPort);
  }
  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    x = strtoul(endptr, &endptr, 16);
    y = strtoul(endptr, &endptr, 16);
    if (!simulate) {
      Flags = 0;
      z = fpMul(x, y);
      printf("%08X %08X %08X %02X\n", x, y, z, Flags);
    } else {
      Flags_sim = 0;
      z = fpMul_sim(x, y);
      printf("%08X %08X %08X %02X\n", x, y, z, Flags_sim);
    }
  }
  if (!simulate) {
    serialClose();
  }
  return 0;
}
