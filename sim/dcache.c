/*
 * dcache.c -- data cache simulation
 *             direct-mapped or two-way associative
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
  int lru;			/* least recently used cache line */
  CacheLine line_0;		/* first cache line in set */
  CacheLine line_1;		/* second cache line in set */
} CacheSet;


static Bool debug = false;

static int ldTotalSize;		/* ld(total size in bytes) */
static int ldLineSize;		/* ld(line size in bytes) */
static int ldAssoc;		/* 0 (direct) or 1 (two-way) */
static int ldSets;		/* ld(number of sets) */

static int totalSize;		/* total size in bytes */
static int lineSize;		/* line size in bytes */
static int assoc;		/* 1 (direct) or 2 (two-way) */
static int sets;		/* number of sets */

static unsigned int offsetMask;	/* mask for offset bits */
static int indexShift;		/* == number of offset bits */
static unsigned int indexMask;	/* mask for index bits (shifted to 0) */
static int tagShift;		/* == number of (index + offset) bits */
static unsigned int tagMask;	/* mask for tag bits (shifted to 0) */

static CacheSet *cache;		/* the cache is an array of cache sets */

static long readAccesses;	/* number of read accesses */
static long readMisses;		/* number of read misses */
static long writeAccesses;	/* number of write accesses */
static long writeMisses;	/* number of write misses */
static long memoryWrites;	/* number of lines written back */


/**************************************************************/


static void readLineFromMemory(Word pAddr, Word *dst) {
  if (debug) {
    cPrintf("**** dcache read from mem: pAddr 0x%08X ****\n", pAddr);
  }
  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~offsetMask, dst, lineSize >> 2);
  } else {
    ramRead(pAddr & ~offsetMask, dst, lineSize >> 2);
  }
}


static void writeLineToMemory(Word pAddr, Word *src) {
  if (debug) {
    cPrintf("**** dcache write to mem: pAddr 0x%08X ****\n", pAddr);
  }
  if ((pAddr & 0x30000000) == ROM_BASE) {
    romWrite(pAddr & ~offsetMask, src, lineSize >> 2);
  } else {
    ramWrite(pAddr & ~offsetMask, src, lineSize >> 2);
  }
}


static Word *getWordPtr(Word pAddr, Bool write) {
  unsigned int tag;
  unsigned int index;
  unsigned int offset;
  Bool hit0, hit1;
  CacheLine *cacheLine;
  Word memAddr;

  /* compute tag, index, and offset */
  tag = (pAddr >> tagShift) & tagMask;
  index = (pAddr >> indexShift) & indexMask;
  offset = pAddr & offsetMask;
  /* cache lookup */
  hit0 = cache[index].line_0.valid && cache[index].line_0.tag == tag;
  if (ldAssoc) {
    hit1 = cache[index].line_1.valid && cache[index].line_1.tag == tag;
  } else {
    hit1 = false;
  }
  if (debug) {
    cPrintf("**** dcache tag = 0x%08X, index = 0x%04X, offset = 0x%02X",
            tag, index, offset);
    cPrintf(" : %s%s (%s) ****\n",
            hit0 ? "hit" : "miss",
            ldAssoc ? (hit1 ? "/hit" : "/miss") : "",
            write ? "wr" : "rd");
  }
  if (hit0) {
    /* cache hit in line_0 */
    cacheLine = &cache[index].line_0;
    cache[index].lru = 1;
  } else
  if (hit1) {
    /* cache hit in line_1 */
    cacheLine = &cache[index].line_1;
    cache[index].lru = 0;
  } else {
    /* cache miss */
    if (ldAssoc == 0 || cache[index].lru == 0) {
      cacheLine = &cache[index].line_0;
      cache[index].lru = 1;
    } else {
      cacheLine = &cache[index].line_1;
      cache[index].lru = 0;
    }
    if (cacheLine->valid && cacheLine->dirty) {
      /* handle write-back */
      memAddr = (cacheLine->tag << tagShift) | (index << indexShift);
      writeLineToMemory(memAddr, cacheLine->data);
      memoryWrites++;
    }
    readLineFromMemory(pAddr, cacheLine->data);
    cacheLine->valid = true;
    cacheLine->dirty = false;
    cacheLine->tag = tag;
    if (write) {
      writeMisses++;
    } else {
      readMisses++;
    }
  }
  if (write) {
    /* handle cache write */
    cacheLine->dirty = true;
    writeAccesses++;
  } else {
    /* handle cache read */
    readAccesses++;
  }
  /* cached data is (now) present */
  return cacheLine->data + (offset >> 2);
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
    cache[index].lru = 0;
    cache[index].line_0.valid = false;
    if (ldAssoc) {
      cache[index].line_1.valid = false;
    }
  }
}


void dcacheFlush(void) {
  unsigned int index;
  CacheLine *cacheLine;
  Word memAddr;

  if (debug) {
    cPrintf("**** dcache flush ****\n");
  }
  for (index = 0; index < sets; index++) {
    cacheLine = &cache[index].line_0;
    if (cacheLine->valid && cacheLine->dirty) {
      /* handle write-back from line_0 */
      memAddr = (cacheLine->tag << tagShift) | (index << indexShift);
      writeLineToMemory(memAddr, cacheLine->data);
      cacheLine->dirty = false;
      memoryWrites++;
    }
    if (ldAssoc) {
      cacheLine = &cache[index].line_1;
      if (cacheLine->valid && cacheLine->dirty) {
        /* handle write-back from line_1 */
        memAddr = (cacheLine->tag << tagShift) | (index << indexShift);
        writeLineToMemory(memAddr, cacheLine->data);
        cacheLine->dirty = false;
        memoryWrites++;
      }
    }
  }
}


/**************************************************************/


long dcacheGetReadAccesses(void) {
  return readAccesses;
}


long dcacheGetReadMisses(void) {
  return readMisses;
}


long dcacheGetWriteAccesses(void) {
  return writeAccesses;
}


long dcacheGetWriteMisses(void) {
  return writeMisses;
}


long dcacheGetMemoryWrites(void) {
  return memoryWrites;
}


/**************************************************************/


Bool dcacheProbe(Word pAddr, Word *data, Bool *dirty) {
  unsigned int tag;
  unsigned int index;
  unsigned int offset;
  Bool hit0, hit1;

  /* compute tag, index, and offset */
  tag = (pAddr >> tagShift) & tagMask;
  index = (pAddr >> indexShift) & indexMask;
  offset = pAddr & offsetMask;
  /* cache lookup */
  hit0 = cache[index].line_0.valid && cache[index].line_0.tag == tag;
  if (ldAssoc) {
    hit1 = cache[index].line_1.valid && cache[index].line_1.tag == tag;
  } else {
    hit1 = false;
  }
  if (hit0) {
    /* return data word and dirty flag from line_0 */
    *data = cache[index].line_0.data[offset >> 2];
    *dirty = cache[index].line_0.dirty;
  } else
  if (hit1) {
    /* return data word and dirty flag from line_1 */
    *data = cache[index].line_1.data[offset >> 2];
    *dirty = cache[index].line_1.dirty;
  }
  return hit0 || hit1;
}


/**************************************************************/


void dcacheReset(void) {
  cPrintf("Resetting Data Cache...\n");
  cPrintf("%6d sets * %d lines/set * %d bytes/line = %d bytes installed.\n",
          sets, assoc, lineSize, totalSize);
  dcacheInvalidate();
  readAccesses = 0;
  readMisses = 0;
  writeAccesses = 0;
  writeMisses = 0;
  memoryWrites = 0;
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
    if (ldAssoc) {
      cache[index].line_1.data = malloc(lineSize);
      if (cache[index].line_1.data == NULL) {
        error("cannot allocate dcache data");
      }
    }
  }
  dcacheReset();
}


void dcacheExit(void) {
  unsigned int index;

  for (index = 0; index < sets; index++) {
    free(cache[index].line_0.data);
    if (ldAssoc) {
      free(cache[index].line_1.data);
    }
  }
  free(cache);
}
