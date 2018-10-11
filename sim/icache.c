/*
 * icache.c -- instruction cache simulation
 *             direct-mapped or two-way associative
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "icache.h"
#include "ram.h"
#include "rom.h"


typedef struct {
  Bool valid;			/* is the line valid? */
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


/**************************************************************/


static void readLineFromMemory(Word pAddr, Word *dst) {
  if (debug) {
    cPrintf("**** icache read from mem: pAddr 0x%08X ****\n", pAddr);
  }
  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr & ~offsetMask, dst, lineSize >> 2);
  } else {
    ramRead(pAddr & ~offsetMask, dst, lineSize >> 2);
  }
}


static Word *getWordPtr(Word pAddr) {
  unsigned int tag;
  unsigned int index;
  unsigned int offset;
  Bool hit0, hit1;
  CacheLine *cacheLine;

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
    cPrintf("**** icache tag = 0x%08X, index = 0x%04X, offset = 0x%02X",
            tag, index, offset);
    cPrintf(" : %s%s ****\n",
            hit0 ? "hit" : "miss",
            ldAssoc ? (hit1 ? "/hit" : "/miss") : "");
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
    readLineFromMemory(pAddr, cacheLine->data);
    cacheLine->valid = true;
    cacheLine->tag = tag;
    readMisses++;
  }
  readAccesses++;
  /* cached data is (now) present */
  return cacheLine->data + (offset >> 2);
}


/**************************************************************/


Word icacheReadWord(Word pAddr) {
  Word *p;
  Word data;

  p = getWordPtr(pAddr);
  data = *p;
  return data;
}


/**************************************************************/


void icacheInvalidate(void) {
  unsigned int index;

  if (debug) {
    cPrintf("**** icache invalidate ****\n");
  }
  for (index = 0; index < sets; index++) {
    cache[index].lru = 0;
    cache[index].line_0.valid = false;
    if (ldAssoc) {
      cache[index].line_1.valid = false;
    }
  }
}


/**************************************************************/


long icacheGetReadAccesses(void) {
  return readAccesses;
}


long icacheGetReadMisses(void) {
  return readMisses;
}


/**************************************************************/


Bool icacheProbe(Word pAddr, Word *data) {
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
    /* return data word from line_0 */
    *data = cache[index].line_0.data[offset >> 2];
  } else
  if (hit1) {
    /* return data word from line_1 */
    *data = cache[index].line_1.data[offset >> 2];
  }
  return hit0 || hit1;
}


/**************************************************************/


void icacheReset(void) {
  cPrintf("Resetting Instruction Cache...\n");
  cPrintf("%6d sets * %d lines/set * %d bytes/line = %d bytes installed.\n",
          sets, assoc, lineSize, totalSize);
  icacheInvalidate();
  readAccesses = 0;
  readMisses = 0;
}


void icacheInit(int ldTotal, int ldLine, int ldAss) {
  unsigned int index;

  ldTotalSize = ldTotal;
  ldLineSize = ldLine;
  ldAssoc = ldAss;
  ldSets = ldTotalSize - ldLineSize - ldAssoc;
  if (ldSets < 0) {
    error("impossible icache geometry");
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
    cPrintf("**** icache tag    : shift = %2d, ", tagShift);
    cPrintf("bits = %2d, mask = 0x%08X ****\n", NPAB - tagShift, tagMask);
    cPrintf("**** icache index  : shift = %2d, ", indexShift);
    cPrintf("bits = %2d, mask = 0x%08X ****\n", ldSets, indexMask);
    cPrintf("**** icache offset : shift = %2d, ", 0);
    cPrintf("bits = %2d, mask = 0x%08X ****\n", ldLineSize, offsetMask);
  }
  cache = malloc(sets * sizeof(CacheSet));
  if (cache == NULL) {
    error("cannot allocate icache");
  }
  for (index = 0; index < sets; index++) {
    cache[index].line_0.data = malloc(lineSize);
    if (cache[index].line_0.data == NULL) {
      error("cannot allocate icache data");
    }
    if (ldAssoc) {
      cache[index].line_1.data = malloc(lineSize);
      if (cache[index].line_1.data == NULL) {
        error("cannot allocate icache data");
      }
    }
  }
  icacheReset();
}


void icacheExit(void) {
  unsigned int index;

  for (index = 0; index < sets; index++) {
    free(cache[index].line_0.data);
    if (ldAssoc) {
      free(cache[index].line_1.data);
    }
  }
  free(cache);
}
