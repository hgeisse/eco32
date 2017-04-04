/*
 * dsksdc.c -- disk made available by SD card interface
 */


#include "common.h"
#include "stdarg.h"
#include "romlib.h"


/**************************************************************/


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


Bool debug = false;


/**************************************************************/


static unsigned int lastSent = 0;


static void select(void) {
  lastSent |= SDC_SELECT;
  *SDC_CTRL = lastSent;
}


static void deselect(void) {
  lastSent &= ~SDC_SELECT;
  *SDC_CTRL = lastSent;
}


static void fastClock(void) {
  lastSent |= SDC_FASTCLK;
  *SDC_CTRL = lastSent;
}


static void slowClock(void) {
  lastSent &= ~SDC_FASTCLK;
  *SDC_CTRL = lastSent;
}


static unsigned char sndRcv(unsigned char b) {
  unsigned char r;

  *SDC_DATA = b;
  while ((*SDC_CTRL & SDC_READY) == 0) ;
  r = *SDC_DATA;
  return r;
}


static void resetCRC7(void) {
  *SDC_CRC7 = 0x01;
}


static unsigned char getCRC7(void) {
  return *SDC_CRC7;
}


static void resetCRC16(Bool fromMISO) {
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


static unsigned short getCRC16(void) {
  return *SDC_CRC16;
}


/**************************************************************/


static int sndCmd(unsigned char *cmd,
                  unsigned char *rcv, int size) {
  int i;
  unsigned char r;

  select();
  resetCRC7();
  for (i = 0; i < 5; i++) {
    (void) sndRcv(cmd[i]);
  }
  (void) sndRcv(getCRC7());
  i = 8;
  do {
    r = sndRcv(0xFF);
  } while (r == 0xFF && --i > 0);
  if (i == 0) {
    return 0;
  }
  rcv[0] = r;
  for (i = 1; i < size; i++) {
    rcv[i] = sndRcv(0xFF);
  }
  deselect();
  (void) sndRcv(0xFF);
  return size;
}


static int sndCmdRcvData(unsigned char *cmd,
                         unsigned char *rcv,
                         unsigned char *ptr, int size) {
  int i;
  unsigned char r;
  unsigned short crc16;

  select();
  resetCRC7();
  for (i = 0; i < 5; i++) {
    (void) sndRcv(cmd[i]);
  }
  (void) sndRcv(getCRC7());
  i = 8;
  do {
    r = sndRcv(0xFF);
  } while (r == 0xFF && --i > 0);
  if (i == 0) {
    return 0;
  }
  rcv[0] = r;
  i = 2048;
  do {
    r = sndRcv(0xFF);
  } while (r != 0xFE && --i > 0);
  if (i == 0) {
    return 0;
  }
  resetCRC16(true);
  for (i = 0; i < size; i++) {
    ptr[i] = sndRcv(0xFF);
  }
  (void) sndRcv(0xFF);
  (void) sndRcv(0xFF);
  crc16 = getCRC16();
  deselect();
  (void) sndRcv(0xFF);
  if (crc16 != 0x0000) {
    /* CRC error */
    return 0;
  }
  return size;
}


/**************************************************************/


static void initCard(void) {
  int i;

  slowClock();
  deselect();
  for (i = 0; i < 10; i++) {
    (void) sndRcv(0xFF);
  }
}


static void resetCard(void) {
  unsigned char cmd[5] = { 0x40, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;

  n = sndCmd(cmd, rcv, 1);
  if (debug) {
    printf("CMD0   (0x00000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
}


static void sendInterfaceCondition(void) {
  unsigned char cmd[5] = { 0x48, 0x00, 0x00, 0x01, 0xAA };
  unsigned char rcv[5];
  int n;

  n = sndCmd(cmd, rcv, 5);
  if (debug) {
    printf("CMD8   (0x000001AA) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
             rcv[0], rcv[1], rcv[2], rcv[3], rcv[4]);
    }
  }
}


static void setCrcOptionOn(void) {
  unsigned char cmd[5] = { 0x7B, 0x00, 0x00, 0x00, 0x01 };
  unsigned char rcv[1];
  int n;

  n = sndCmd(cmd, rcv, 1);
  if (debug) {
    printf("CMD59  (0x00000001) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
}


static void prepAppCmd(void) {
  unsigned char cmd[5] = { 0x77, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;

  n = sndCmd(cmd, rcv, 1);
  if (debug) {
    printf("CMD55  (0x00000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
}


static unsigned char sendHostCap(void) {
  unsigned char cmd[5] = { 0x69, 0x40, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;

  n = sndCmd(cmd, rcv, 1);
  if (debug) {
    printf("ACMD41 (0x40000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
  return n == 0 ? 0xFF : rcv[0];
}


static void activateCard(void) {
  int tries;
  unsigned char res;

  for (tries = 0; tries < 1000; tries++) {
    prepAppCmd();
    res = sendHostCap();
    if (res == 0x00) {
      /* SD card activated */
      return;
    }
  }
  /* cannot activate SD card */
}


static void readOCR(void) {
  unsigned char cmd[5] = { 0x7A, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[5];
  int n;

  n = sndCmd(cmd, rcv, 5);
  if (debug) {
    printf("CMD58  (0x00000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
             rcv[0], rcv[1], rcv[2], rcv[3], rcv[4]);
    }
  }
}


static void sendCardSpecData(unsigned char *csd) {
  unsigned char cmd[5] = { 0x49, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;
  int i;

  n = sndCmdRcvData(cmd, rcv, csd, 16);
  if (debug) {
    printf("CMD9   (0x00000000) : ");
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
    for (i = 0; i < 8; i++) {
      printf("0x%02X  ", csd[i]);
    }
    printf("\n");
    for (i = 8; i < 16; i++) {
      printf("0x%02X  ", csd[i]);
    }
    printf("\n");
  }
}


static void readSingleBlock(unsigned int sct, unsigned char *ptr) {
  unsigned char cmd[5] = { 0x51, 0x00, 0x00, 0x00, 0x00 };
  unsigned char rcv[1];
  int n;

  cmd[1] = (sct >> 24) & 0xFF;
  cmd[2] = (sct >> 16) & 0xFF;
  cmd[3] = (sct >>  8) & 0xFF;
  cmd[4] = (sct >>  0) & 0xFF;
  n = sndCmdRcvData(cmd, rcv, ptr, 512);
  if (debug) {
    printf("CMD51  (0x%02X%02X%02X%02X) : ",
           cmd[1], cmd[2], cmd[3], cmd[4]);
    if (n == 0) {
      printf("no answer\n");
    } else {
      printf("0x%02X\n", rcv[0]);
    }
  }
}


/**************************************************************/


static Bool sdcInitialized = false;


static void initSDC(void) {
  initCard();
  resetCard();
  sendInterfaceCondition();
  setCrcOptionOn();
  activateCard();
  readOCR();
  fastClock();
  sdcInitialized = true;
}


/**************************************************************/


static unsigned int numSectors = 0;


void dskinitsdc(void) {
  sdcInitialized = false;
}


int dskcapsdc(void) {
  unsigned char csd[16];
  unsigned int csize;

  if (!sdcInitialized) {
    initSDC();
  }
  sendCardSpecData(csd);
  if ((csd[0] & 0xC0) != 0x40) {
    /* wrong CSD structure version */
    return 0;
  }
  csize = (((unsigned int) csd[7] & 0x3F) << 16) |
          (((unsigned int) csd[8] & 0xFF) <<  8) |
          (((unsigned int) csd[9] & 0xFF) <<  0);
  numSectors = (csize + 1) << 10;
  return numSectors;
}


int dskiosdc(char cmd, int sct, Word addr, int nscts) {
  int i;
  unsigned char *ptr;

  if (!sdcInitialized) {
    initSDC();
  }
  if (cmd == 'r') {
    ptr = (unsigned char *) (0xC0000000 | addr);
    for (i = 0; i < nscts; i++) {
      if (sct >= numSectors) {
        return -1;
      }
      readSingleBlock(sct, ptr);
      sct++;
      ptr += 512;
    }
    return 0;
  }
  if (cmd == 'w') {
    return -1;
  }
  return -1;
}
