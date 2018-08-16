/*
 * sdcard.c -- SD card simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "timer.h"
#include "sdcard.h"


static Bool debugRead = false;
static Bool debugWrite = false;
static Bool debugCallback = false;
static Bool debugCommand = false;

static Bool installed = false;

static FILE *sdcardImage;
static long totalSectors;

static Word status;
static Word control;
static Word dataReg;
static Word crc7Reg;
static unsigned char crc7;
static Word crc16Reg;
static unsigned short crc16;


/**************************************************************/


static unsigned char dataBuf[SDC_SECTOR_SIZE];


static void sectorRead(unsigned int sctno) {
  fseek(sdcardImage, sctno * SDC_SECTOR_SIZE, SEEK_SET);
  if (fread(dataBuf, SDC_SECTOR_SIZE, 1, sdcardImage) != 1) {
    error("cannot read from SD card image");
  }
}


static void sectorWrite(unsigned int sctno) {
  fseek(sdcardImage, sctno * SDC_SECTOR_SIZE, SEEK_SET);
  if (fwrite(dataBuf, SDC_SECTOR_SIZE, 1, sdcardImage) != 1) {
    error("cannot write to SD card image");
  }
}


/**************************************************************/


/*
 * Note: After receiving a command, crc7 is NOT zero because of
 * the additional '1' bit appended to each command. A single '1'
 * transforms the checksum from 0 to the divisor polynomial, which
 * is found at crc7_table[1]. This does not hold for data transfers,
 * i.e. crc16 must be zero after receiving a data sector.
 */


static unsigned char crc7_table[256] = {
  0x00, 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3F,
  0x48, 0x41, 0x5A, 0x53, 0x6C, 0x65, 0x7E, 0x77,
  0x19, 0x10, 0x0B, 0x02, 0x3D, 0x34, 0x2F, 0x26,
  0x51, 0x58, 0x43, 0x4A, 0x75, 0x7C, 0x67, 0x6E,
  0x32, 0x3B, 0x20, 0x29, 0x16, 0x1F, 0x04, 0x0D,
  0x7A, 0x73, 0x68, 0x61, 0x5E, 0x57, 0x4C, 0x45,
  0x2B, 0x22, 0x39, 0x30, 0x0F, 0x06, 0x1D, 0x14,
  0x63, 0x6A, 0x71, 0x78, 0x47, 0x4E, 0x55, 0x5C,
  0x64, 0x6D, 0x76, 0x7F, 0x40, 0x49, 0x52, 0x5B,
  0x2C, 0x25, 0x3E, 0x37, 0x08, 0x01, 0x1A, 0x13,
  0x7D, 0x74, 0x6F, 0x66, 0x59, 0x50, 0x4B, 0x42,
  0x35, 0x3C, 0x27, 0x2E, 0x11, 0x18, 0x03, 0x0A,
  0x56, 0x5F, 0x44, 0x4D, 0x72, 0x7B, 0x60, 0x69,
  0x1E, 0x17, 0x0C, 0x05, 0x3A, 0x33, 0x28, 0x21,
  0x4F, 0x46, 0x5D, 0x54, 0x6B, 0x62, 0x79, 0x70,
  0x07, 0x0E, 0x15, 0x1C, 0x23, 0x2A, 0x31, 0x38,
  0x41, 0x48, 0x53, 0x5A, 0x65, 0x6C, 0x77, 0x7E,
  0x09, 0x00, 0x1B, 0x12, 0x2D, 0x24, 0x3F, 0x36,
  0x58, 0x51, 0x4A, 0x43, 0x7C, 0x75, 0x6E, 0x67,
  0x10, 0x19, 0x02, 0x0B, 0x34, 0x3D, 0x26, 0x2F,
  0x73, 0x7A, 0x61, 0x68, 0x57, 0x5E, 0x45, 0x4C,
  0x3B, 0x32, 0x29, 0x20, 0x1F, 0x16, 0x0D, 0x04,
  0x6A, 0x63, 0x78, 0x71, 0x4E, 0x47, 0x5C, 0x55,
  0x22, 0x2B, 0x30, 0x39, 0x06, 0x0F, 0x14, 0x1D,
  0x25, 0x2C, 0x37, 0x3E, 0x01, 0x08, 0x13, 0x1A,
  0x6D, 0x64, 0x7F, 0x76, 0x49, 0x40, 0x5B, 0x52,
  0x3C, 0x35, 0x2E, 0x27, 0x18, 0x11, 0x0A, 0x03,
  0x74, 0x7D, 0x66, 0x6F, 0x50, 0x59, 0x42, 0x4B,
  0x17, 0x1E, 0x05, 0x0C, 0x33, 0x3A, 0x21, 0x28,
  0x5F, 0x56, 0x4D, 0x44, 0x7B, 0x72, 0x69, 0x60,
  0x0E, 0x07, 0x1C, 0x15, 0x2A, 0x23, 0x38, 0x31,
  0x46, 0x4F, 0x54, 0x5D, 0x62, 0x6B, 0x70, 0x79,
};


static unsigned char crc7_update(unsigned char crc7,
                                 unsigned char data) {
  return crc7_table[(crc7 << 1) ^ data];
}


/**************************************************************/


static unsigned short crc16_table[256] = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
  0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
  0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
  0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
  0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
  0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
  0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
  0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
  0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
  0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
  0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
  0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
  0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
  0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
  0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
  0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
  0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
  0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
  0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
  0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
  0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
  0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
  0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
};


static unsigned short crc16_update(unsigned short crc16,
                                   unsigned char data) {
  return (crc16 << 8) ^ crc16_table[(crc16 >> 8) ^ data];
}


/**************************************************************/


static unsigned char csd[16] = {
  0x40, 0x0E, 0x00, 0x32, 0x5B, 0x59, 0x00, 0x00,
  0x00, 0x00, 0x7F, 0x80, 0x0A, 0x40, 0x40, 0xC3,
};

static unsigned char cid[16] = {
  0x03, 0x53, 0x44, 0x53, 0x4C, 0x33, 0x32, 0x47,
  0x80, 0x69, 0x0D, 0x44, 0x29, 0x01, 0x0B, 0x1D,
};


/**************************************************************/


#define ST_ACCUM_CMD	0
#define ST_INTERP_CMD	1
#define ST_ANSWER	2
#define ST_RD_DELAY	3
#define ST_RD_DATA	4
#define ST_RD_CRC	5
#define ST_WR_WAIT	6
#define ST_WR_DATA	7
#define ST_WR_CRC	8
#define ST_WR_RSP	9
#define ST_WR_BUSY	10


#define PRM_INIT_COUNT	10	/* realistic value: 389 */
#define SEC_INIT_COUNT	5	/* realistic value: 20 */
#define CSD_DELAY_COUNT	2	/* realistic value: ?? */
#define RD_DELAY_COUNT	5	/* realistic value: 150 */
#define WR_BUSY_COUNT	7	/* realistic value: 170 */


static int state;
static int currCnt;
static unsigned char cmd[6];
static Bool idle;
static int xpctCnt;
static int dataCnt;
static unsigned char ans[4];
static Bool crcChecked;
static Bool appCmdSeen;
static int initCnt;
static unsigned int sctno;


static void sdcardCallback(int n) {
  int i;

  if (debugCallback) {
    cPrintf("\n**** SDCARD CALLBACK, state = %d ****\n", state);
  }
  if ((control & SDC_CTRL_SELECT) == 0) {
    dataReg = 0xFF;
    status |= SDC_STAT_READY;
    return;
  }
  switch (state) {
    case ST_ACCUM_CMD:
      if (currCnt < 6) {
        cmd[currCnt] = dataReg;
        currCnt++;
        crc7 = crc7_update(crc7, dataReg);
        dataReg = 0xFF;
        status |= SDC_STAT_READY;
      } else {
        dataReg = 0xFF;
        status |= SDC_STAT_READY;
        state = ST_INTERP_CMD;
      }
      break;
    case ST_INTERP_CMD:
      if (debugCommand) {
        cPrintf("SDCARD cmd = 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
                cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
      }
      currCnt = 0;
      state = ST_ACCUM_CMD;
      dataReg = 0x00;
      status |= SDC_STAT_READY;
      if (crcChecked && crc7 != crc7_table[1]) {
        dataReg |= SDC_ANS_COM_CRC_ERR;
      } else
      if (!appCmdSeen && cmd[0] == 0x40) {
        /* CMD0: reset */
        idle = true;
        crcChecked = false;
      } else
      if (!appCmdSeen && cmd[0] == 0x48) {
        /* CMD8: send interface condition */
        /* response R7 */
        xpctCnt = 4;
        ans[0] = 0x00;
        ans[1] = 0x00;
        ans[2] = 0x01;
        ans[3] = cmd[4];
        state = ST_ANSWER;
      } else
      if (!appCmdSeen && cmd[0] == 0x7B) {
        /* CMD59: turn CRC on/off */
        crcChecked = (cmd[4] & 1) ? true : false;
      } else
      if (!appCmdSeen && cmd[0] == 0x77) {
        /* CMD55: application command follows */
        appCmdSeen = true;
      } else
      if (appCmdSeen && cmd[0] == 0x69) {
        /* ACMD41: initialize SD card */
        appCmdSeen = false;
        initCnt--;
        if (initCnt == 0) {
          initCnt = SEC_INIT_COUNT;
          idle = false;
        }
      } else
      if (!appCmdSeen && cmd[0] == 0x7A) {
        /* CMD58: read OCR */
        /* response R3 */
        xpctCnt = 4;
        ans[0] = 0xC0;
        ans[1] = 0xFF;
        ans[2] = 0x80;
        ans[3] = 0x00;
        state = ST_ANSWER;
      } else
      if (!appCmdSeen && cmd[0] == 0x49) {
        /* CMD9: send card-specific data */
        for (i = 0; i < 16; i++) {
          dataBuf[i] = csd[i];
        }
        xpctCnt = CSD_DELAY_COUNT;
        dataCnt = 16;
        state = ST_RD_DELAY;
      } else
      if (!appCmdSeen && cmd[0] == 0x4A) {
        /* CMD10: send card identification data */
        for (i = 0; i < 16; i++) {
          dataBuf[i] = cid[i];
        }
        xpctCnt = CSD_DELAY_COUNT;
        dataCnt = 16;
        state = ST_RD_DELAY;
      } else
      if (!appCmdSeen && cmd[0] == 0x51) {
        /* CMD17: read single sector */
        sctno = ((unsigned int) cmd[1] << 24) |
                ((unsigned int) cmd[2] << 16) |
                ((unsigned int) cmd[3] <<  8) |
                ((unsigned int) cmd[4] <<  0);
        if (sctno < totalSectors) {
          /* valid address */
          sectorRead(sctno);
          xpctCnt = RD_DELAY_COUNT;
          dataCnt = SDC_SECTOR_SIZE;
          state = ST_RD_DELAY;
        } else {
          /* address out of range */
          dataReg |= SDC_ANS_PARAM_ERR;
        }
      } else
      if (!appCmdSeen && cmd[0] == 0x58) {
        /* CMD24: write single sector */
        sctno = ((unsigned int) cmd[1] << 24) |
                ((unsigned int) cmd[2] << 16) |
                ((unsigned int) cmd[3] <<  8) |
                ((unsigned int) cmd[4] <<  0);
        if (sctno < totalSectors) {
          /* valid address */
          state = ST_WR_WAIT;
        } else {
          /* address out of range */
          dataReg |= SDC_ANS_PARAM_ERR;
        }
      } else {
        /* illegal command */
        dataReg |= SDC_ANS_ILLEGAL_CMD;
      }
      if (idle) {
        dataReg |= SDC_ANS_IN_IDLE_STATE;
      }
      break;
    case ST_ANSWER:
      dataReg = ans[currCnt];
      status |= SDC_STAT_READY;
      currCnt++;
      if (currCnt == xpctCnt) {
        currCnt = 0;
        state = ST_ACCUM_CMD;
      }
      break;
    case ST_RD_DELAY:
      dataReg = 0xFF;
      status |= SDC_STAT_READY;
      currCnt++;
      if (currCnt == xpctCnt) {
        dataReg = 0xFE;
        currCnt = 0;
        xpctCnt = dataCnt;
        state = ST_RD_DATA;
      }
      break;
    case ST_RD_DATA:
      if (control & SDC_CTRL_CRC16MISO) {
        /* bits from MISO */
        crc16 = crc16_update(crc16, dataBuf[currCnt]);
      } else {
        /* bits from MOSI */
        crc16 = crc16_update(crc16, dataReg & 0xFF);
      }
      dataReg = dataBuf[currCnt];
      status |= SDC_STAT_READY;
      currCnt++;
      if (currCnt == xpctCnt) {
        currCnt = 0;
        xpctCnt = 2;
        ans[0] = (crc16 >> 8) & 0xFF;
        ans[1] = crc16 & 0xFF;
        state = ST_RD_CRC;
      }
      break;
    case ST_RD_CRC:
      if (control & SDC_CTRL_CRC16MISO) {
        /* bits from MISO */
        crc16 = crc16_update(crc16, ans[currCnt]);
      } else {
        /* bits from MOSI */
        crc16 = crc16_update(crc16, dataReg & 0xFF);
      }
      dataReg = ans[currCnt];
      status |= SDC_STAT_READY;
      currCnt++;
      if (currCnt == xpctCnt) {
        currCnt = 0;
        state = ST_ACCUM_CMD;
      }
      break;
    case ST_WR_WAIT:
      if (dataReg == 0xFE) {
        currCnt = 0;
        xpctCnt = SDC_SECTOR_SIZE;
        state = ST_WR_DATA;
      }
      dataReg = 0xFF;
      status |= SDC_STAT_READY;
      break;
    case ST_WR_DATA:
      dataBuf[currCnt] = dataReg & 0xFF;
      if (control & SDC_CTRL_CRC16MISO) {
        /* bits from MISO */
        crc16 = crc16_update(crc16, 0xFF);
      } else {
        /* bits from MOSI */
        crc16 = crc16_update(crc16, dataReg & 0xFF);
      }
      dataReg = 0xFF;
      status |= SDC_STAT_READY;
      currCnt++;
      if (currCnt == xpctCnt) {
        currCnt = 0;
        xpctCnt = 2;
        state = ST_WR_CRC;
      }
      break;
    case ST_WR_CRC:
      if (control & SDC_CTRL_CRC16MISO) {
        /* bits from MISO */
        crc16 = crc16_update(crc16, 0xFF);
      } else {
        /* bits from MOSI */
        crc16 = crc16_update(crc16, dataReg & 0xFF);
      }
      dataReg = 0xFF;
      status |= SDC_STAT_READY;
      currCnt++;
      if (currCnt == xpctCnt) {
        state = ST_WR_RSP;
      }
      break;
    case ST_WR_RSP:
      dataReg = 0xE0;
      if (crc16 == 0x0000) {
        sectorWrite(sctno);
        dataReg |= 0x05;
      } else {
        dataReg |= 0x0B;
      }
      status |= SDC_STAT_READY;
      currCnt = 0;
      xpctCnt = WR_BUSY_COUNT;
      state = ST_WR_BUSY;
      break;
    case ST_WR_BUSY:
      dataReg = 0x00;
      status |= SDC_STAT_READY;
      currCnt++;
      if (currCnt == xpctCnt) {
        dataReg = 0xFF;
        currCnt = 0;
        state = ST_ACCUM_CMD;
      }
      break;
    default:
      error("illegal state %d in SD card callback", state);
      break;
  }
}


/**************************************************************/


Word sdcardRead(Word addr) {
  Word data;

  if (debugRead) {
    cPrintf("\n**** SDCARD READ from 0x%08X", addr);
  }
  if (!installed) {
    /* SD card controller not installed */
    throwException(EXC_BUS_TIMEOUT);
  }
  switch ((addr >> 2) & 3) {
    case 0:
      /* read status */
      data = status;
      break;
    case 1:
      /* read rcv data */
      data = dataReg;
      status &= ~SDC_STAT_READY;
      break;
    case 2:
      /* read CRC7 */
      crc7Reg &= 0xFFFFFF01;
      crc7Reg |= (Word) crc7 << 1;
      data = crc7Reg;
      break;
    case 3:
      /* read CRC16 */
      crc16Reg &= 0xFFFF0000;
      crc16Reg |= (Word) crc16;
      data = crc16Reg;
      break;
  }
  if (debugRead) {
    cPrintf(", data = 0x%08X ****\n", data);
  }
  return data;
}


void sdcardWrite(Word addr, Word data) {
  if (debugWrite) {
    cPrintf("\n**** SDCARD WRITE to 0x%08X, data = 0x%08X ****\n",
            addr, data);
  }
  if (!installed) {
    /* SD card controller not installed */
    throwException(EXC_BUS_TIMEOUT);
  }
  switch ((addr >> 2) & 3) {
    case 0:
      /* write control */
      control = data & 0x07;
      break;
    case 1:
      /* write xmt data */
      dataReg = data & 0xFF;
      timerStart(1, sdcardCallback, 0);
      break;
    case 2:
      /* write CRC7 */
      crc7Reg = data & 0xFF;
      crc7 = (unsigned char) (crc7Reg >> 1);
      break;
    case 3:
      /* write CRC16 */
      crc16Reg = data & 0xFFFF;
      crc16 = (unsigned short) data;
      break;
  }
}


void sdcardReset(void) {
  if (!installed) {
    /* SD card controller not installed */
    return;
  }
  cPrintf("Resetting SD card...\n");
  status = 0;
  control = 0;
  dataReg = 0;
  crc7Reg = 0;
  crc7 = 0;
  crc16Reg = 0;
  crc16 = 0;
  state = ST_ACCUM_CMD;
  currCnt = 0;
  idle = true;
  crcChecked = false;
  appCmdSeen = false;
  initCnt = PRM_INIT_COUNT;
  if (totalSectors != 0) {
    cPrintf("SD card of size %ld sectors (%ld bytes) installed.\n",
            totalSectors, totalSectors * SDC_SECTOR_SIZE);
  }
}


void sdcardInit(char *sdcardImageName) {
  long numBytes;
  unsigned int csize;

  if (sdcardImageName == NULL) {
    /* do not install SD card */
    sdcardImage = NULL;
    totalSectors = 0;
  } else {
    /* try to install SD card */
    sdcardImage = fopen(sdcardImageName, "r+b");
    if (sdcardImage == NULL) {
      error("cannot open SD card image '%s'", sdcardImageName);
    }
    fseek(sdcardImage, 0, SEEK_END);
    numBytes = ftell(sdcardImage);
    fseek(sdcardImage, 0, SEEK_SET);
    if (numBytes % (1024 * SDC_SECTOR_SIZE) != 0) {
      cPrintf("Warning: SD card image '%s' ", sdcardImageName);
      cPrintf("is not a multiple of 1024 sectors\n");
    }
    totalSectors = numBytes / SDC_SECTOR_SIZE;
    csize = totalSectors / 1024 - 1;
    csd[7] = (csize >> 16) & 0x3F;
    csd[8] = (csize >>  8) & 0xFF;
    csd[9] = (csize >>  0) & 0xFF;
  }
  installed = true;
  sdcardReset();
}


void sdcardExit(void) {
  if (!installed) {
    /* SD card controller not installed */
    return;
  }
  if (sdcardImage == NULL) {
    /* SD card not installed */
    return;
  }
  fclose(sdcardImage);
}
