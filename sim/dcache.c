/*
 * dcache.c -- data cache simulation
 *             method: fake, does not cache anything
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "dcache.h"
#include "ram.h"
#include "rom.h"


Word dcacheReadWord(Word pAddr) {
  Word temp;
  Word data;

  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~3, &temp, 1);
  } else {
    ramRead(pAddr & ~3, &temp, 1);
  }
  data = temp;
  return data;
}


Half dcacheReadHalf(Word pAddr) {
  Word temp;
  Half data;

  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~3, &temp, 1);
  } else {
    ramRead(pAddr & ~3, &temp, 1);
  }
  switch (pAddr & 2) {
    case 0:
      data = (Half) (temp >> 16);
      break;
    case 2:
      data = (Half) (temp >>  0);
      break;
  }
  return data;
}


Byte dcacheReadByte(Word pAddr) {
  Word temp;
  Byte data;

  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~3, &temp, 1);
  } else {
    ramRead(pAddr & ~3, &temp, 1);
  }
  switch (pAddr & 3) {
    case 0:
      data = (Byte) (temp >> 24);
      break;
    case 1:
      data = (Byte) (temp >> 16);
      break;
    case 2:
      data = (Byte) (temp >>  8);
      break;
    case 3:
      data = (Byte) (temp >>  0);
      break;
  }
  return data;
}


void dcacheWriteWord(Word pAddr, Word data) {
  Word temp;

  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~3, &temp, 1);
  } else {
    ramRead(pAddr & ~3, &temp, 1);
  }
  temp = data;
  if ((pAddr & 0x30000000) == ROM_BASE) {
    romWrite(pAddr & ~3, &temp, 1);
  } else {
    ramWrite(pAddr & ~3, &temp, 1);
  }
}


void dcacheWriteHalf(Word pAddr, Half data) {
  Word temp;

  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~3, &temp, 1);
  } else {
    ramRead(pAddr & ~3, &temp, 1);
  }
  switch (pAddr & 2) {
    case 0:
      temp &= ~(0xFFFF << 16);
      temp |= ((Word) data) << 16;
      break;
    case 2:
      temp &= ~(0xFFFF <<  0);
      temp |= ((Word) data) <<  0;
      break;
  }
  if ((pAddr & 0x30000000) == ROM_BASE) {
    romWrite(pAddr & ~3, &temp, 1);
  } else {
    ramWrite(pAddr & ~3, &temp, 1);
  }
}


void dcacheWriteByte(Word pAddr, Byte data) {
  Word temp;

  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~3, &temp, 1);
  } else {
    ramRead(pAddr & ~3, &temp, 1);
  }
  switch (pAddr & 3) {
    case 0:
      temp &= ~(0xFF << 24);
      temp |= ((Word) data) << 24;
      break;
    case 1:
      temp &= ~(0xFF << 16);
      temp |= ((Word) data) << 16;
      break;
    case 2:
      temp &= ~(0xFF <<  8);
      temp |= ((Word) data) <<  8;
      break;
    case 3:
      temp &= ~(0xFF <<  0);
      temp |= ((Word) data) <<  0;
      break;
  }
  if ((pAddr & 0x30000000) == ROM_BASE) {
    romWrite(pAddr & ~3, &temp, 1);
  } else {
    ramWrite(pAddr & ~3, &temp, 1);
  }
}


void dcacheInvalidate(void) {
}


void dcacheFlush(void) {
}


void dcacheReset(void) {
  cPrintf("Resetting Data Cache...\n");
}


void dcacheInit(void) {
  dcacheReset();
}


void dcacheExit(void) {
}
