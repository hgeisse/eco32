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


#define KBD_BASE	((Word *) 0xF0200000)


int kbdFlag = 0;
int key;


void setLeds(int leds) {
  while (((*(KBD_BASE + 0)) & 0x10) == 0) ;
  *(KBD_BASE + 1) = (char) 0xED;
  while (kbdFlag == 0) ;
  kbdFlag = 0;
  while (((*(KBD_BASE + 0)) & 0x10) == 0) ;
  *(KBD_BASE + 1) = (char) leds;
  while (kbdFlag == 0) ;
  kbdFlag = 0;
}


int kbdISR(int irq) {
  kbdFlag = 1;
  key = *(KBD_BASE + 1);
  return 0;
}


void initKbd(void) {
  Word mask;

  *(KBD_BASE + 0) = 2;
  setISR(4, kbdISR);
  mask = getMask();
  setMask(mask | (1 << 4));
}


/**************************************************************/


#define TMR_BASE	((Word *) 0xF0000000)


int tmrFlag = 0;


int tmrISR(int irq) {
  tmrFlag = *(TMR_BASE + 0);
  return 0;
}


void initTmr(void) {
  Word mask;

  *(TMR_BASE + 1) = 20000000;
  *(TMR_BASE + 0) = 2;
  setISR(14, tmrISR);
  mask = getMask();
  setMask(mask | (1 << 14));
}


/**************************************************************/


int ledMap[8] = {
  0, 1, 4, 5, 2, 3, 6, 7
};


void main(void) {
  int ledIndex;
  int n;

  initInterrupts();
  initKbd();
  initTmr();
  enable();
  printf("\n");
  printf("Please press a few keys on the PS/2 keyboard...\n");
  ledIndex = 0;
  n = 0;
  while (1) {
    if (tmrFlag) {
      tmrFlag = 0;
      setLeds(ledMap[ledIndex]);
      ledIndex = (ledIndex + 1) & 7;
    }
    if (kbdFlag) {
      kbdFlag = 0;
      printf("%02x ", key);
      if (++n == 24) {
        n = 0;
        printf("\n");
      }
    }
  }
}
