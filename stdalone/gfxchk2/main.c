/*
 * main.c -- start the ball rolling
 */


#include "stdarg.h"
#include "start.h"


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


int defaultISR(int irq) {
  printf("\n%s\n", exceptionCause[irq]);
  return 0;  /* do not skip any instruction */
}


void initInterrupts(void) {
  int i;

  for (i = 0; i < 32; i++) {
    setISR(i, defaultISR);
  }
}


/**************************************************************/


#define BASE	((unsigned int *) 0xF5000000)


void setPixel(int x, int y, int on) {
  unsigned int *waddr;
  unsigned int pmask;
  unsigned int pixels;

  waddr = BASE + ((y << 5) | (x >> 5));
  pmask = 0x80000000 >> (x & 0x1F);
  pixels = *waddr;
  if (on) {
    pixels |= pmask;
  } else {
    pixels &= ~pmask;
  }
  *waddr = pixels;
}


void clearScreen(void) {
  unsigned int *waddr;
  int count;

  waddr = BASE;
  count = 32 * 768;
  while (count--) {
    *waddr++ = 0x00000000;
  }
}


/**************************************************************/


int abs(int x) {
  return x < 0 ? -x : x;
}


void drawLine(int x0, int y0, int x1, int y1, int on) {
  int dx, sx;
  int dy, sy;
  int err, err2;

  dx = abs(x1 - x0);
  sx = (x0 < x1) ? 1 : -1;
  dy = -abs(y1 - y0);
  sy = (y0 < y1) ? 1 : -1;
  err = dx + dy;
  while (1) {
    setPixel(x0, y0, on);
    if (x0 == x1 && y0 == y1) {
      break;
    }
    err2 = 2 * err;
    if (err2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (err2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}


/**************************************************************/


#define BACKGROUND	0
#define FOREGROUND	1


void main(void) {
  initInterrupts();
  clearScreen();
  drawLine(0, 0, 1023, 767, FOREGROUND);
  drawLine(0, 767, 1023, 0, FOREGROUND);
  drawLine(0, 383, 1023, 383, FOREGROUND);
  drawLine(0, 384, 1023, 384, FOREGROUND);
  drawLine(511, 0, 511, 767, FOREGROUND);
  drawLine(512, 0, 512, 767, FOREGROUND);
  printf("\nHalting...\n");
}
