/*
 * main.c -- ECO32 SD card interface test
 */


#include "types.h"
#include "stdarg.h"
#include "iolib.h"


#define SDC_BASE	((unsigned int *) 0xF0600000)

#define CTRL_000	0x00	/* SS_N = 0, MOSI = 0, SCLK = 0 */
#define CTRL_001	0x20	/* SS_N = 0, MOSI = 0, SCLK = 1 */
#define CTRL_010	0x10	/* SS_N = 0, MOSI = 1, SCLK = 0 */
#define CTRL_011	0x30	/* SS_N = 0, MOSI = 1, SCLK = 1 */
#define CTRL_100	0x08	/* SS_N = 1, MOSI = 0, SCLK = 0 */
#define CTRL_101	0x28	/* SS_N = 1, MOSI = 0, SCLK = 1 */
#define CTRL_110	0x18	/* SS_N = 1, MOSI = 1, SCLK = 0 */
#define CTRL_111	0x38	/* SS_N = 1, MOSI = 1, SCLK = 1 */


/**************************************************************/


#define LED_BASE	((unsigned int *) 0xF1000020)


void ledOn(void) {
  *LED_BASE = 0x00000001;
}


void ledOff(void) {
  *LED_BASE = 0x00000000;
}


/**************************************************************/


void interfaceActive(void) {
  ledOn();
  *SDC_BASE = 0x00000718;
}


void interfacePassive(void) {
  *SDC_BASE = 0x0000FFFF;
  ledOff();
}


void interfacePut(unsigned char b) {
  *SDC_BASE = 0x0700 | (unsigned int) b;
}


unsigned char interfaceGet(void) {
  return (unsigned char) (*SDC_BASE & 0x007F);
}


void select(void) {
  interfacePut(CTRL_010);
}


void deselect(void) {
  interfacePut(CTRL_110);
}


void toggleClock(int n) {
  while (n--) {
    interfacePut(CTRL_111);
    interfacePut(CTRL_110);
  }
}


unsigned char sndRcv(unsigned char b) {
  int i;
  unsigned char r;

  r = 0x00;
  for (i = 0; i < 8; i++) {
    r <<= 1;
    if (b & 0x80) {
      interfacePut(CTRL_010);
      r |= interfaceGet() & 1;
      interfacePut(CTRL_011);
    } else {
      interfacePut(CTRL_000);
      r |= interfaceGet() & 1;
      interfacePut(CTRL_001);
    }
    b <<= 1;
  }
  interfacePut(CTRL_010);
  return r;
}


/**************************************************************/


void test(void) {
  int i;
  unsigned char rcv1, rcv2, rcv3, rcv4, rcv5;
  unsigned char sector[512];
  int j, k;
  unsigned char c;

  interfaceActive();
  toggleClock(80);
  /**********/
  select();
  rcv1 = sndRcv(0x40);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x95);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  deselect();
  toggleClock(8);
  printf("CMD0 (0) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x\n", rcv1);
  }
  /**********/
  select();
  rcv1 = sndRcv(0x48);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x01);
  rcv1 = sndRcv(0xAA);
  rcv1 = sndRcv(0x87);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  rcv4 = sndRcv(0xFF);
  rcv5 = sndRcv(0xFF);
  deselect();
  toggleClock(8);
  printf("CMD8 (0x1AA) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
           rcv1, rcv2, rcv3, rcv4, rcv5);
  }
  /**********/
again:
  select();
  rcv1 = sndRcv(0x77);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x65);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  deselect();
  toggleClock(8);
  printf("CMD55 (0) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x\n", rcv1);
  }
  /**********/
  select();
  rcv1 = sndRcv(0x69);
  rcv1 = sndRcv(0x40);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x77);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  deselect();
  toggleClock(8);
  printf("ACMD41 (0x40000000) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x\n", rcv1);
  }
  if (rcv1 == 0x01) goto again;
  /**********/
  select();
  rcv1 = sndRcv(0x7A);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0xFD);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  rcv4 = sndRcv(0xFF);
  rcv5 = sndRcv(0xFF);
  deselect();
  toggleClock(8);
  printf("CMD58 (0) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
           rcv1, rcv2, rcv3, rcv4, rcv5);
  }
  /**********/
  select();
  rcv1 = sndRcv(0x51);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x00);
  rcv1 = sndRcv(0x07);
  rcv1 = sndRcv(0xC2);
  rcv1 = sndRcv(0x00);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  do {
    rcv2 = sndRcv(0xFF);
  } while (rcv2 != 0xFE);
  for (j = 0; j < 512; j++) {
    sector[j] = sndRcv(0xFF);
  }
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  deselect();
  toggleClock(8);
  printf("CMD17 (0x000007C2) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x\n", rcv1);
    for (j = 0; j < 32; j++) {
      printf("%03x:  ", 16 * j);
      for (k = 0; k < 16; k++) {
        printf("%02x ", sector[16 * j + k]);
      }
      printf("   ");
      for (k = 0; k < 16; k++) {
        c = sector[16 * j + k];
        if (c < 0x20 || c > 0x7E) {
          printf(".");
        } else {
          printf("%c", c);
        }
      }
      printf("\n");
    }
    printf("CRC    = 0x%02x%02x\n", rcv3, rcv4);
  }
  /**********/
  interfacePassive();
}


/**************************************************************/


int main(void) {
  printf("\nECO32 SD card interface test started\n\n");
  test();
  printf("\nECO32 SD card interface test finished\n");
  return 0;
}
