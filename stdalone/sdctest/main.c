/*
 * main.c -- ECO32 SD card interface test
 */


#include "types.h"
#include "stdarg.h"
#include "iolib.h"


/* SDC addresses */
#define SDC_BASE	((unsigned int *) 0xF0600000)
#define SDC_CTRL	(SDC_BASE + 0)
#define SDC_DATA	(SDC_BASE + 1)
#define SDC_CRC7	(SDC_BASE + 2)
#define SDC_CRC16	(SDC_BASE + 3)

/* SDC control bits */
#define SDC_CRC16MISO	((unsigned int) 0x04)
#define SDC_FASTCLK	((unsigned int) 0x02)
#define SDC_SELECT	((unsigned int) 0x01)

/* SDC status bits */
#define SDC_WPROT	((unsigned int) 0x02)
#define SDC_READY	((unsigned int) 0x01)


/**************************************************************/


#define LED_BASE	((unsigned int *) 0xF1000020)


void ledOn(void) {
  *LED_BASE = 0x00000001;
}


void ledOff(void) {
  *LED_BASE = 0x00000000;
}


/**************************************************************/


unsigned int lastSent = 0;


void select(void) {
  lastSent |= SDC_SELECT;
  *SDC_CTRL = lastSent;
}


void deselect(void) {
  lastSent &= ~SDC_SELECT;
  *SDC_CTRL = lastSent;
}


void fastClk(void) {
  lastSent |= SDC_FASTCLK;
  *SDC_CTRL = lastSent;
}


void slowClk(void) {
  lastSent &= ~SDC_FASTCLK;
  *SDC_CTRL = lastSent;
}


unsigned char sndRcv(unsigned char b) {
  unsigned char r;

  *SDC_DATA = b;
  while ((*SDC_CTRL & SDC_READY) == 0) ;
  r = *SDC_DATA;
  return r;
}


void resetCRC7(void) {
  *SDC_CRC7 = 0x01;
}


unsigned char getCRC7(void) {
  return *SDC_CRC7;
}


void resetCRC16(Bool fromMISO) {
  if (fromMISO) {
    /* compute CRC for bits from MISO */
    lastSent |= SDC_CRC16MISO;
    *SDC_CTRL = lastSent;
  } else {
    /* compute CRC for bits from MOSI */
    lastSent &= ~SDC_CRC16MISO;
    *SDC_CTRL = lastSent;
  }
  *SDC_CRC16 = 0x0000;
}


unsigned short getCRC16(void) {
  return *SDC_CRC16;
}


/**************************************************************/


void test(void) {
  int i;
  unsigned char dummy;
  unsigned char rcv1, rcv2, rcv3, rcv4, rcv5;
  unsigned char sector[512];
  int j, k;
  unsigned char c;
  unsigned char crc7;
  unsigned short crc16;

  /***********************************/
  /* initialize                      */
  /***********************************/
  slowClk();
  deselect();
  for (i = 0; i < 10; i++) {
    dummy = sndRcv(0xFF);
  }
  /***********************************/
  /* reset                           */
  /***********************************/
  select();
  resetCRC7();
  dummy = sndRcv(0x40);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  deselect();
  dummy = sndRcv(0xFF);
  printf("CMD0 (0) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x\n", rcv1);
  }
  /***********************************/
  /* send interface condition        */
  /***********************************/
  select();
  resetCRC7();
  dummy = sndRcv(0x48);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x01);
  dummy = sndRcv(0xAA);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  rcv4 = sndRcv(0xFF);
  rcv5 = sndRcv(0xFF);
  deselect();
  dummy = sndRcv(0xFF);
  printf("CMD8 (0x1AA) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
           rcv1, rcv2, rcv3, rcv4, rcv5);
  }
  /***********************************/
  /* set CRC option to ON            */
  /***********************************/
  select();
  resetCRC7();
  dummy = sndRcv(0x7B);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x01);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  deselect();
  dummy = sndRcv(0xFF);
  printf("CMD59 (0) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x\n", rcv1);
  }
  /***********************************/
again:
  /***********************************/
  /* prepare app-specific cmd        */
  /***********************************/
  select();
  resetCRC7();
  dummy = sndRcv(0x77);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  deselect();
  dummy = sndRcv(0xFF);
  printf("CMD55 (0) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x\n", rcv1);
  }
  /***********************************/
  /* send host capacity support      */
  /***********************************/
  select();
  resetCRC7();
  dummy = sndRcv(0x69);
  dummy = sndRcv(0x40);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  deselect();
  dummy = sndRcv(0xFF);
  printf("ACMD41 (0x40000000) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x\n", rcv1);
  }
  /****************************************/
  /* repeat as long as in idle state      */
  /****************************************/
  if (rcv1 == 0x01) goto again;
  /****************************************/
  /* read OCR                             */
  /****************************************/
  select();
  resetCRC7();
  dummy = sndRcv(0x7A);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  rcv4 = sndRcv(0xFF);
  rcv5 = sndRcv(0xFF);
  deselect();
  dummy = sndRcv(0xFF);
  printf("CMD58 (0) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
           rcv1, rcv2, rcv3, rcv4, rcv5);
  }
  /***********************************/
  /* switch to fast clock            */
  /***********************************/
  fastClk();
  /***********************************/
  /* read single block               */
  /***********************************/
  select();
  resetCRC7();
  dummy = sndRcv(0x51);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x00);
  dummy = sndRcv(0x07);
  dummy = sndRcv(0xC2);
  //dummy = sndRcv(0xC3);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  do {
    rcv2 = sndRcv(0xFF);
  } while (rcv2 != 0xFE);
  resetCRC16(TRUE);
  for (j = 0; j < 512; j++) {
    sector[j] = sndRcv(0xFF);
  }
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  crc16 = getCRC16();
  deselect();
  dummy = sndRcv(0xFF);
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
    printf("CRC check : 0x%04x\n", crc16);
  }
  /***********************************/
  /* read 2048 blocks, slow clock    */
  /***********************************/
  printf("slow read of 1 MB started\n");
  slowClk();
  for (k = 0; k < 2048; k++) {
    select();
    resetCRC7();
    dummy = sndRcv(0x51);
    dummy = sndRcv(0x00);
    dummy = sndRcv(0x00);
    dummy = sndRcv((k >> 8) & 0x00FF);
    dummy = sndRcv((k >> 0) & 0x00FF);
    crc7 = getCRC7();
    dummy = sndRcv(crc7);
    i = 8;
    do {
      rcv1 = sndRcv(0xFF);
    } while (rcv1 == 0xFF && --i > 0);
    do {
      rcv2 = sndRcv(0xFF);
    } while (rcv2 != 0xFE);
    resetCRC16(TRUE);
    for (j = 0; j < 512; j++) {
      sector[j] = sndRcv(0xFF);
    }
    rcv2 = sndRcv(0xFF);
    rcv3 = sndRcv(0xFF);
    crc16 = getCRC16();
    deselect();
    dummy = sndRcv(0xFF);
  }
  printf("slow read done\n");
  /***********************************/
  /* read 2048 blocks, fast clock    */
  /***********************************/
  printf("fast read of 1 MB started\n");
  fastClk();
  for (k = 0; k < 2048; k++) {
    select();
    resetCRC7();
    dummy = sndRcv(0x51);
    dummy = sndRcv(0x00);
    dummy = sndRcv(0x00);
    dummy = sndRcv((k >> 8) & 0x00FF);
    dummy = sndRcv((k >> 0) & 0x00FF);
    crc7 = getCRC7();
    dummy = sndRcv(crc7);
    i = 8;
    do {
      rcv1 = sndRcv(0xFF);
    } while (rcv1 == 0xFF && --i > 0);
    do {
      rcv2 = sndRcv(0xFF);
    } while (rcv2 != 0xFE);
    resetCRC16(TRUE);
    for (j = 0; j < 512; j++) {
      sector[j] = sndRcv(0xFF);
    }
    rcv2 = sndRcv(0xFF);
    rcv3 = sndRcv(0xFF);
    crc16 = getCRC16();
    deselect();
    dummy = sndRcv(0xFF);
  }
  printf("fast read done\n");
}


/**************************************************************/


int main(void) {
  printf("\nECO32 SD card interface test started\n\n");
  ledOn();
  test();
  ledOff();
  printf("\nECO32 SD card interface test finished\n");
  return 0;
}
