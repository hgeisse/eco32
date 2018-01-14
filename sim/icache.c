/*
 * icache.c -- instruction cache simulation
 *             direct mapped (working)
 *             two way associative (not working)
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


#define IC_LD_TOTAL_SIZE	12	/* ld(total size in bytes) */
#define IC_LD_LINE_SIZE		4	/* ld(line size in bytes) */
#define IC_TWO_WAY_ASSOC	false	/* false: direct mapped */
					/* true: two way associative */

#define PAB			30	/* physical address bits */


typedef struct {
  Bool valid;			/* is the line valid? */
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
    cPrintf("**** icache read from mem: index 0x%04X @ pAddr 0x%08X ****\n",
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


static Word *getWordPtr(Word pAddr) {
  unsigned int tag;
  unsigned int index;
  unsigned int offset;
  Bool hit;

  /* compute tag, index, and offset */
  tag = (pAddr >> tagShift) & tagMask;
  index = (pAddr >> indexShift) & indexMask;
  offset = pAddr & offsetMask;
  /* cache lookup */
  hit = cache[index].line_0.valid && cache[index].line_0.tag == tag;
  if (debug) {
    cPrintf("**** icache tag = 0x%08X, index = 0x%04X, offset = 0x%02X",
            tag, index, offset);
    cPrintf(" : %s ****\n", hit ? "hit" : "miss");
  }
  if (!hit) {
    /* handle cache miss */
    readLineFromMemory(pAddr, index);
    cache[index].line_0.valid = true;
    cache[index].line_0.tag = tag;
  }
  /* cached data is (now) present */
  return cache[index].line_0.data + (offset >> 2);
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
    cache[index].line_0.valid = false;
  }
}


void icacheReset(void) {
  cPrintf("Resetting Instruction Cache...\n");
  cPrintf("%6d sets * %d lines/set * %d bytes/line = %d bytes installed.\n",
          sets, assoc, lineSize, totalSize);
  icacheInvalidate();
}


void icacheInit(void) {
  unsigned int index;

  ldTotalSize = IC_LD_TOTAL_SIZE;
  ldLineSize = IC_LD_LINE_SIZE;
  ldAssoc = IC_TWO_WAY_ASSOC ? 1 : 0;
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
  tagMask = (1 << (PAB - tagShift)) - 1;
  if (debug) {
    cPrintf("**** icache tag    : shift = %2d, ", tagShift);
    cPrintf("bits = %2d, mask = 0x%08X ****\n", PAB - tagShift, tagMask);
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
  }
  icacheReset();
}


void icacheExit(void) {
  unsigned int index;

  for (index = 0; index < sets; index++) {
    free(cache[index].line_0.data);
  }
  free(cache);
}
