/*
 * dcache.c -- data cache simulation
 *             direct mapped (working)
 *             two way associative (not working)
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


typedef struct {
  Bool valid;			/* is the line valid? */
  Bool dirty;			/* is the line dirty? */
  unsigned int tag;		/* upper address bits */
  Word *data;			/* line data as an array of words */
} CacheLine;

typedef struct {
  CacheLine line_0;		/* first cache line in set */
  /*CacheLine line_1;*/		/* second cache line in set */
} CacheSet;


static Bool debug = false;

static int ldTotalSize;		/* ld(total size in bytes) */
static int ldLineSize;		/* ld(line size in bytes) */
static int ldAssoc;		/* 0 (direct) or 1 (two way) */
static int ldSets;		/* ld(number of sets) */

static int totalSize;		/* total size in bytes */
static int lineSize;		/* line size in bytes */
static int assoc;		/* 1 (direct) or 2 (two way) */
static int sets;		/* number of sets */

static unsigned int offsetMask;	/* mask for offset bits */
static int indexShift;		/* == number of offset bits */
static unsigned int indexMask;	/* mask for index bits (shifted to 0) */
static int tagShift;		/* == number of (index + offset) bits */
static unsigned int tagMask;	/* mask for tag bits (shifted to 0) */

static CacheSet *cache;		/* the cache is an array of cache sets */


/**************************************************************/


static void readLineFromMemory(Word pAddr, unsigned int index) {
  if (debug) {
    cPrintf("**** dcache read from mem: index 0x%04X @ pAddr 0x%08X ****\n",
            index, pAddr);
  }
  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~offsetMask,
            cache[index].line_0.data,
            lineSize >> 2);
  } else {
    ramRead(pAddr & ~offsetMask,
            cache[index].line_0.data,
            lineSize >> 2);
  }
}


static void writeLineToMemory(Word pAddr, unsigned int index) {
  if (debug) {
    cPrintf("**** dcache write to mem: index 0x%04X @ pAddr 0x%08X ****\n",
            index, pAddr);
  }
  if ((pAddr & 0x30000000) == ROM_BASE) {
    romWrite(pAddr & ~offsetMask,
             cache[index].line_0.data,
             lineSize >> 2);
  } else {
    ramWrite(pAddr & ~offsetMask,
             cache[index].line_0.data,
             lineSize >> 2);
  }
}


static Word *getWordPtr(Word pAddr, Bool write) {
  unsigned int tag;
  unsigned int index;
  unsigned int offset;
  Bool hit;
  Word memAddr;

  /* compute tag, index, and offset */
  tag = (pAddr >> tagShift) & tagMask;
  index = (pAddr >> indexShift) & indexMask;
  offset = pAddr & offsetMask;
  /* cache lookup */
  hit = cache[index].line_0.valid && cache[index].line_0.tag == tag;
  if (debug) {
    cPrintf("**** dcache tag = 0x%08X, index = 0x%04X, offset = 0x%02X",
            tag, index, offset);
    cPrintf(" : %s (%s) ****\n",
            hit ? "hit" : "miss", write ? "wr" : "rd");
  }
  if (!hit) {
    /* handle cache miss */
    if (cache[index].line_0.valid & cache[index].line_0.dirty) {
      /* handle write back */
      memAddr = (cache[index].line_0.tag << tagShift) |
                (index << indexShift);
      writeLineToMemory(memAddr, index);
    }
    readLineFromMemory(pAddr, index);
    cache[index].line_0.valid = true;
    cache[index].line_0.dirty = false;
    cache[index].line_0.tag = tag;
  }
  if (write) {
    /* handle cache write */
    cache[index].line_0.dirty = true;
  }
  /* cached data is (now) present */
  return cache[index].line_0.data + (offset >> 2);
}


/**************************************************************/


Word dcacheReadWord(Word pAddr) {
  Word *p;
  Word data;

  p = getWordPtr(pAddr, false);
  data = *p;
  return data;
}


Half dcacheReadHalf(Word pAddr) {
  Word *p;
  Word temp;
  Half data;

  p = getWordPtr(pAddr, false);
  temp = *p;
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
  Word *p;
  Word temp;
  Byte data;

  p = getWordPtr(pAddr, false);
  temp = *p;
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
  Word *p;

  p = getWordPtr(pAddr, true);
  *p = data;
}


void dcacheWriteHalf(Word pAddr, Half data) {
  Word *p;
  Word temp;

  p = getWordPtr(pAddr, true);
  temp = *p;
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
  *p = temp;
}


void dcacheWriteByte(Word pAddr, Byte data) {
  Word *p;
  Word temp;

  p = getWordPtr(pAddr, true);
  temp = *p;
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
  *p = temp;
}


/**************************************************************/


void dcacheInvalidate(void) {
  unsigned int index;

  if (debug) {
    cPrintf("**** dcache invalidate ****\n");
  }
  for (index = 0; index < sets; index++) {
    cache[index].line_0.valid = false;
  }
}


void dcacheFlush(void) {
  unsigned int index;
  Word memAddr;

  if (debug) {
    cPrintf("**** dcache flush ****\n");
  }
  for (index = 0; index < sets; index++) {
    if (cache[index].line_0.valid & cache[index].line_0.dirty) {
      memAddr = (cache[index].line_0.tag << tagShift) |
                (index << indexShift);
      writeLineToMemory(memAddr, index);
      cache[index].line_0.dirty = false;
    }
  }
}


void dcacheReset(void) {
  cPrintf("Resetting Data Cache...\n");
  cPrintf("%6d sets * %d lines/set * %d bytes/line = %d bytes installed.\n",
          sets, assoc, lineSize, totalSize);
  dcacheInvalidate();
}


void dcacheInit(int ldTotal, int ldLine, int ldAss) {
  unsigned int index;

  ldTotalSize = ldTotal;
  ldLineSize = ldLine;
  ldAssoc = ldAss;
  ldSets = ldTotalSize - ldLineSize - ldAssoc;
  if (ldSets < 0) {
    error("impossible dcache geometry");
  }
  totalSize = 1 << ldTotalSize;
  lineSize = 1 << ldLineSize;
  assoc = 1 << ldAssoc;
  sets = 1 << ldSets;
  offsetMask = lineSize - 1;
  indexShift = ldLineSize;
  indexMask = sets - 1;
  tagShift = ldSets + ldLineSize;
  tagMask = (1 << (NPAB - tagShift)) - 1;
  if (debug) {
    cPrintf("**** dcache tag    : shift = %2d, ", tagShift);
    cPrintf("bits = %2d, mask = 0x%08X ****\n", NPAB - tagShift, tagMask);
    cPrintf("**** dcache index  : shift = %2d, ", indexShift);
    cPrintf("bits = %2d, mask = 0x%08X ****\n", ldSets, indexMask);
    cPrintf("**** dcache offset : shift = %2d, ", 0);
    cPrintf("bits = %2d, mask = 0x%08X ****\n", ldLineSize, offsetMask);
  }
  cache = malloc(sets * sizeof(CacheSet));
  if (cache == NULL) {
    error("cannot allocate dcache");
  }
  for (index = 0; index < sets; index++) {
    cache[index].line_0.data = malloc(lineSize);
    if (cache[index].line_0.data == NULL) {
      error("cannot allocate dcache data");
    }
  }
  dcacheReset();
}


void dcacheExit(void) {
  unsigned int index;

  for (index = 0; index < sets; index++) {
    free(cache[index].line_0.data);
  }
  free(cache);
}
