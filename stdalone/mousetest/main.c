/*
 * main.c -- start the ball rolling
 */


#include "types.h"
#include "stdarg.h"
#include "iolib.h"
#include "start.h"


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


#define MOUSE_BASE	((Word *) 0xF0201000)
#define MOUSE_BUFSIZE	3000
#define MOUSE_INC(x)	((x) == MOUSE_BUFSIZE - 1 ? 0 : (x) + 1)


Byte mouseBuf[MOUSE_BUFSIZE];
int mouseWrPtr = 0;
int mouseRdPtr = 0;


int mouseISR(int irq) {
  Byte b;

  b = *(MOUSE_BASE + 1);
  if (MOUSE_INC(mouseWrPtr) != mouseRdPtr) {
    mouseBuf[mouseWrPtr] = b;
    mouseWrPtr = MOUSE_INC(mouseWrPtr);
  }
  return 0;
}


Bool mouseHasByte(void) {
  return mouseRdPtr != mouseWrPtr;
}


Byte mouseGetByte(void) {
  Byte b;

  while (!mouseHasByte()) ;
  b = mouseBuf[mouseRdPtr];
  mouseRdPtr = MOUSE_INC(mouseRdPtr);
  return b;
}


void mouseGetPacket(Byte *p) {
  *(p + 0) = mouseGetByte();
  *(p + 1) = mouseGetByte();
  *(p + 2) = mouseGetByte();
}


void mouseDrainInput(void) {
  int count;

  count = 0;
  while (1) {
    if (mouseHasByte()) {
      (void) mouseGetByte();
      count = 0;
    } else {
      count++;
      if (count == 100000) {
        return;
      }
    }
  }
}


Bool mouseExpect(Byte x) {
  int count;
  Byte b;

  count = 0;
  while (1) {
    if (mouseHasByte()) {
      break;
    }
    count++;
    if (count == 100000) {
      return FALSE;
    }
  }
  b = mouseGetByte();
  //printf("received %02x, expected %02x\n", b, x);
  return b == x;
}


void mouseReset(void) {
  while (1) {
    /* reset */
    while (1) {
      /* drain input */
      mouseDrainInput();
      /* send reset command */
      while (((*(MOUSE_BASE + 0)) & 0x10) == 0) ;
      *(MOUSE_BASE + 1) = (char) 0xFF;
      /* check answer */
      if (mouseExpect(0xFA) &&
          mouseExpect(0xAA) &&
          mouseExpect(0x00)) {
        break;
      }
    }
    /* enable data reporting */
    while (((*(MOUSE_BASE + 0)) & 0x10) == 0) ;
    *(MOUSE_BASE + 1) = (char) 0xF4;
    if (mouseExpect(0xFA)) {
      return;
    }
  }
}


void mouseInit(void) {
  Word mask;
  Byte b;

  *(MOUSE_BASE + 0) = 2;
  setISR(5, mouseISR);
  mask = getMask();
  setMask(mask | (1 << 5));
  enable();
  mouseReset();
}


/**************************************************************/


void main(void) {
  Byte b[3];
  Bool ox, oy;
  Bool sx, sy;
  Bool must_be_1;
  Bool mb, rb, lb;
  int dx, dy;
  int x, y;

  initInterrupts();
  mouseInit();
  printf("\n");
  printf("Please move the PS/2 mouse!\n");
  printf("Press left mouse button to show\n");
  printf("and reset accumulated dx/dy values.\n");
  x = 0;
  y = 0;
  while (1) {
    mouseGetPacket(b);
    oy = (b[0] & 0x80) ? TRUE : FALSE;
    ox = (b[0] & 0x40) ? TRUE : FALSE;
    sy = (b[0] & 0x20) ? TRUE : FALSE;
    sx = (b[0] & 0x10) ? TRUE : FALSE;
    must_be_1 = (b[0] & 0x08) ? TRUE : FALSE;
    mb = (b[0] & 0x04) ? TRUE : FALSE;
    rb = (b[0] & 0x02) ? TRUE : FALSE;
    lb = (b[0] & 0x01) ? TRUE : FALSE;
    dx = (sx ? 0xFFFFFFF0 : 0x00000000) | (unsigned int) b[1];
    dy = (sy ? 0xFFFFFFF0 : 0x00000000) | (unsigned int) b[2];
    printf("%02x %02x %02x : ", b[0], b[1], b[2]);
    if (ox) {
      printf("dx = %covf, ", sx ? '-' : '+');
    } else {
      printf("dx = %4d, ", dx);
    }
    if (oy) {
      printf("dy = %covf, ", sy ? '-' : '+');
    } else {
      printf("dy = %4d, ", dy);
    }
    printf("(%c), ", must_be_1 ? '!' : '?');
    printf("btns = %c%c%c",
           lb ? 'L' : '-',
           mb ? 'M' : '-',
           rb ? 'R' : '-');
    printf("\n");
    x += dx;
    y += dy;
    if (lb) {
      printf("total movement since last query: x = %d, y = %d\n",
             x, y);
      x = 0;
      y = 0;
    }
  }
}
