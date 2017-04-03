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
#define SDC_SELECT	((unsigned int) 0x01)
#define SDC_FASTCLK	((unsigned int) 0x02)
#define SDC_CRC16MISO	((unsigned int) 0x04)

/* SDC status bits */
#define SDC_READY	((unsigned int) 0x01)
#define SDC_WPROT	((unsigned int) 0x02)


#define SPI_LOG		0	/* do NOT enable if any TEST is 1 */
#define SPI_LOG_SIZE	(32 * 1024)

#define TEST_READ_SLOW	0	/* do NOT enable if SPI_LOG is 1 */
#define TEST_READ_FAST	0	/* do NOT enable if SPI_LOG is 1 */


/**************************************************************/


unsigned int randomNumber;


void setRandomNumber(unsigned int seed) {
  randomNumber = seed;
}


unsigned int getRandomNumber(void) {
  randomNumber = randomNumber * (unsigned) 1103515245 + (unsigned) 12345;
  return randomNumber;
}


/**************************************************************/


#if SPI_LOG


int spiLogCount = 0;


unsigned char spiLogData[SPI_LOG_SIZE];


void showLogData(void) {
  int j;

  if (spiLogCount <= SPI_LOG_SIZE - 2) {
    printf("SPI Log Count = %d\n", spiLogCount);
  } else {
    printf("*** SPI Log Overflow ***\n");
  }
  for (j = 0; j < spiLogCount; j++) {
    printf("0x%02X,", spiLogData[j]);
    if (j % 8 != 7) {
      printf(" ");
    } else {
      printf("\n");
    }
  }
}


#endif


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
#if SPI_LOG
  if (spiLogCount <= SPI_LOG_SIZE - 2) {
    spiLogData[spiLogCount++] = b;
    spiLogData[spiLogCount++] = r;
  }
#endif
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


#define START_BLOCK_TOKEN	0xFE


void test(void) {
  int i;
  unsigned char dummy;
  unsigned char rcv1, rcv2, rcv3, rcv4, rcv5;
  unsigned char rdsector[512], wrsector[512];
  int j, k;
  unsigned char c;
  unsigned char crc7;
  unsigned short crc16;
  unsigned char a1[4] = { 0x00, 0x00, 0xE0, 0x01 };
  unsigned char a2[4] = { 0x00, 0x00, 0xE0, 0x02 };

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
#if !SPI_LOG
  printf("CMD0   (0x00000000) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
  }
#endif
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
#if !SPI_LOG
  printf("CMD8   (0x000001AA) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
           rcv1, rcv2, rcv3, rcv4, rcv5);
  }
#endif
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
#if !SPI_LOG
  printf("CMD59  (0x00000001) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
  }
#endif
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
#if !SPI_LOG
  printf("CMD55  (0x00000000) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
  }
#endif
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
#if !SPI_LOG
  printf("ACMD41 (0x40000000) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
  }
#endif
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
#if !SPI_LOG
  printf("CMD58  (0x00000000) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
           rcv1, rcv2, rcv3, rcv4, rcv5);
  }
#endif
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
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  do {
    rcv2 = sndRcv(0xFF);
  } while (rcv2 != START_BLOCK_TOKEN);
  resetCRC16(TRUE);
  for (j = 0; j < 512; j++) {
    rdsector[j] = sndRcv(0xFF);
  }
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  crc16 = getCRC16();
  deselect();
  dummy = sndRcv(0xFF);
#if !SPI_LOG
  printf("CMD17  (0x000007C2) : ");
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
    printf("                   ");
    printf("CRC check = 0x%04X\n", crc16);
    for (j = 0; j < 32; j++) {
      printf("%03X:  ", 16 * j);
      for (k = 0; k < 16; k++) {
        printf("%02X ", rdsector[16 * j + k]);
      }
      printf("   ");
      for (k = 0; k < 16; k++) {
        c = rdsector[16 * j + k];
        if (c < 0x20 || c > 0x7E) {
          printf(".");
        } else {
          printf("%c", c);
        }
      }
      printf("\n");
    }
  }
#endif
  /***********************************/
  /* write block, high address       */
  /***********************************/
  setRandomNumber(10307);
  for (i = 0; i < 512; i++) {
    wrsector[i] = (getRandomNumber() >> 8) & 0xFF;
  }
  select();
  resetCRC7();
  dummy = sndRcv(0x58);
  dummy = sndRcv(a1[0]);
  dummy = sndRcv(a1[1]);
  dummy = sndRcv(a1[2]);
  dummy = sndRcv(a1[3]);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  dummy = sndRcv(START_BLOCK_TOKEN);
  resetCRC16(FALSE);
  for (i = 0; i < 512; i++) {
    dummy = sndRcv(wrsector[i]);
  }
  /* send CRC16 */
  crc16 = getCRC16();
  dummy = sndRcv((crc16 >> 8) & 0xFF);
  dummy = sndRcv((crc16 >> 0) & 0xFF);
  /* get data response token */
  rcv2 = sndRcv(0xFF);
  /* wait while busy */
  do {
    rcv3 = sndRcv(0xFF);
  } while (rcv3 == 0x00);
  deselect();
  dummy = sndRcv(0xFF);
#if !SPI_LOG
  printf("CMD24  (0x%02X%02X%02X%02X) : ",
         a1[0], a1[1], a1[2], a1[3]);
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
    printf("               ");
    printf("data response = 0x%02X (", rcv2);
    if ((rcv2 & 0x1F) == 0x05) {
      printf("accepted");
    } else
    if ((rcv2 & 0x1F) == 0x0B) {
      printf("rejected, CRC error");
    } else
    if ((rcv2 & 0x1F) == 0x0D) {
      printf("rejected, write error");
    } else {
      printf("???");
    }
    printf(")\n");
  }
#endif
  /***********************************/
  /* write block, high address + 1   */
  /***********************************/
  for (i = 0; i < 512; i++) {
    wrsector[i] = (getRandomNumber() >> 8) & 0xFF;
  }
  select();
  resetCRC7();
  dummy = sndRcv(0x58);
  dummy = sndRcv(a2[0]);
  dummy = sndRcv(a2[1]);
  dummy = sndRcv(a2[2]);
  dummy = sndRcv(a2[3]);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  dummy = sndRcv(START_BLOCK_TOKEN);
  resetCRC16(FALSE);
  for (i = 0; i < 512; i++) {
    dummy = sndRcv(wrsector[i]);
  }
  /* send CRC16 */
  crc16 = getCRC16();
  dummy = sndRcv((crc16 >> 8) & 0xFF);
  dummy = sndRcv((crc16 >> 0) & 0xFF);
  /* get data response token */
  rcv2 = sndRcv(0xFF);
  /* wait while busy */
  do {
    rcv3 = sndRcv(0xFF);
  } while (rcv3 == 0x00);
  deselect();
  dummy = sndRcv(0xFF);
#if !SPI_LOG
  printf("CMD24  (0x%02X%02X%02X%02X) : ",
         a2[0], a2[1], a2[2], a2[3]);
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
    printf("               ");
    printf("data response = 0x%02X (", rcv2);
    if ((rcv2 & 0x1F) == 0x05) {
      printf("accepted");
    } else
    if ((rcv2 & 0x1F) == 0x0B) {
      printf("rejected, CRC error");
    } else
    if ((rcv2 & 0x1F) == 0x0D) {
      printf("rejected, write error");
    } else {
      printf("???");
    }
    printf(")\n");
  }
#endif
  /***********************************/
  /* read block, high address        */
  /***********************************/
  setRandomNumber(10307);
  for (i = 0; i < 512; i++) {
    wrsector[i] = (getRandomNumber() >> 8) & 0xFF;
  }
  select();
  resetCRC7();
  dummy = sndRcv(0x51);
  dummy = sndRcv(a1[0]);
  dummy = sndRcv(a1[1]);
  dummy = sndRcv(a1[2]);
  dummy = sndRcv(a1[3]);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  do {
    rcv2 = sndRcv(0xFF);
  } while (rcv2 != START_BLOCK_TOKEN);
  resetCRC16(TRUE);
  for (j = 0; j < 512; j++) {
    rdsector[j] = sndRcv(0xFF);
  }
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  crc16 = getCRC16();
  deselect();
  dummy = sndRcv(0xFF);
#if !SPI_LOG
  printf("CMD17  (0x%02X%02X%02X%02X) : ",
         a1[0], a1[1], a1[2], a1[3]);
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
    printf("                   ");
    printf("CRC check = 0x%04X\n", crc16);
    for (j = 0; j < 32; j++) {
      printf("%03X:  ", 16 * j);
      for (k = 0; k < 16; k++) {
        if (rdsector[16 * j + k] == wrsector[16 * j + k]) {
          printf(".  ");
        } else {
          printf("?  ");
        }
      }
      printf("\n");
    }
  }
#endif
  /***********************************/
  /* read block, high address + 1    */
  /***********************************/
  for (i = 0; i < 512; i++) {
    wrsector[i] = (getRandomNumber() >> 8) & 0xFF;
  }
  select();
  resetCRC7();
  dummy = sndRcv(0x51);
  dummy = sndRcv(a2[0]);
  dummy = sndRcv(a2[1]);
  dummy = sndRcv(a2[2]);
  dummy = sndRcv(a2[3]);
  crc7 = getCRC7();
  dummy = sndRcv(crc7);
  i = 8;
  do {
    rcv1 = sndRcv(0xFF);
  } while (rcv1 == 0xFF && --i > 0);
  do {
    rcv2 = sndRcv(0xFF);
  } while (rcv2 != START_BLOCK_TOKEN);
  resetCRC16(TRUE);
  for (j = 0; j < 512; j++) {
    rdsector[j] = sndRcv(0xFF);
  }
  rcv2 = sndRcv(0xFF);
  rcv3 = sndRcv(0xFF);
  crc16 = getCRC16();
  deselect();
  dummy = sndRcv(0xFF);
#if !SPI_LOG
  printf("CMD17  (0x%02X%02X%02X%02X) : ",
         a2[0], a2[1], a2[2], a2[3]);
  if (i == 0) {
    printf("no answer\n");
  } else {
    printf("answer = 0x%02X\n", rcv1);
    printf("                   ");
    printf("CRC check = 0x%04X\n", crc16);
    for (j = 0; j < 32; j++) {
      printf("%03X:  ", 16 * j);
      for (k = 0; k < 16; k++) {
        if (rdsector[16 * j + k] == wrsector[16 * j + k]) {
          printf(".  ");
        } else {
          printf("?  ");
        }
      }
      printf("\n");
    }
  }
#endif
  /***********************************/
  /* read 2048 blocks, slow clock    */
  /***********************************/
#if TEST_READ_SLOW
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
    } while (rcv2 != START_BLOCK_TOKEN);
    resetCRC16(TRUE);
    for (j = 0; j < 512; j++) {
      rdsector[j] = sndRcv(0xFF);
    }
    rcv2 = sndRcv(0xFF);
    rcv3 = sndRcv(0xFF);
    crc16 = getCRC16();
    deselect();
    dummy = sndRcv(0xFF);
  }
  printf("slow read done\n");
#endif
  /***********************************/
  /* read 2048 blocks, fast clock    */
  /***********************************/
#if TEST_READ_FAST
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
    } while (rcv2 != START_BLOCK_TOKEN);
    resetCRC16(TRUE);
    for (j = 0; j < 512; j++) {
      rdsector[j] = sndRcv(0xFF);
    }
    rcv2 = sndRcv(0xFF);
    rcv3 = sndRcv(0xFF);
    crc16 = getCRC16();
    deselect();
    dummy = sndRcv(0xFF);
  }
  printf("fast read done\n");
#endif
  /***********************************/
  /* show SPI log data               */
  /***********************************/
#if SPI_LOG
  showLogData();
#endif
}


/**************************************************************/


int main(void) {
  printf("\nECO32 SD card interface test started\n\n");
  test();
  printf("\nECO32 SD card interface test finished\n");
  return 0;
}
