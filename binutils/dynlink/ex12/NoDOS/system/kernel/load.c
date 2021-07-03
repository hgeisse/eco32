/*
 * load.c -- program loader
 */


#include "stdarg.h"
#include "iolib.h"
#include "disk.h"
#include "eof.h"
#include "load.h"


static int verbose = 0;


int readObjHeader(EofHeader *hdr,
                  FILE *inFile, char *inPath,
                  unsigned int magic) {
  if (fseek(inFile, 0, SEEK_SET) < 0) {
    return LDERR_HDR;
  }
  if (fread(hdr, sizeof(EofHeader), 1, inFile) != 1) {
    return LDERR_HDR;
  }
  if (hdr->magic != magic) {
    return LDERR_MGC;
  }
  return LDERR_NONE;
}


char *readObjStrings(unsigned int ostrs, unsigned int sstrs,
                     FILE *inFile, char *inPath) {
  char *strs;

  strs = malloc(sstrs);
  if (strs == NULL) {
    return NULL;
  }
  if (fseek(inFile, ostrs, SEEK_SET) < 0) {
    return NULL;
  }
  if (fread(strs, 1, sstrs, inFile) != sstrs) {
    return NULL;
  }
  return strs;
}


int readObjSegment(SegmentRecord *segRec,
                   unsigned int osegs, int segno,
                   FILE *inFile, char *inPath) {
  if (fseek(inFile, osegs + segno * sizeof(SegmentRecord), SEEK_SET) < 0) {
    return LDERR_SEG;
  }
  if (fread(segRec, sizeof(SegmentRecord), 1, inFile) != 1) {
    return LDERR_SEG;
  }
  return LDERR_NONE;
}


void showObjSegment(SegmentRecord *segRec, char *strs) {
  printf("    loading segment '%s'\n", strs + segRec->name);
  printf("        offset  : 0x%08X\n", segRec->offs);
  printf("        address : 0x%08X\n", segRec->addr);
  printf("        size    : 0x%08X\n", segRec->size);
  printf("        attr    : ");
  if (segRec->attr & SEG_ATTR_A) {
    printf("A");
  } else {
    printf("-");
  }
  if (segRec->attr & SEG_ATTR_P) {
    printf("P");
  } else {
    printf("-");
  }
  if (segRec->attr & SEG_ATTR_W) {
    printf("W");
  } else {
    printf("-");
  }
  if (segRec->attr & SEG_ATTR_X) {
    printf("X");
  } else {
    printf("-");
  }
  printf("\n");
}


int loadLinkUnit(char *inPath,
                 unsigned int magic,
                 unsigned int loadOff,
                 unsigned int *startAddr) {
  FILE *inFile;
  int ldr;
  EofHeader hdr;
  char *strs;
  int i;
  SegmentRecord segRec;
  unsigned char *dp;

  /* open input file */
  inFile = fopen(inPath);
  if (inFile == NULL) {
    return LDERR_FND;
  }
  /* read header */
  ldr = readObjHeader(&hdr, inFile, inPath, magic);
  if (ldr != LDERR_NONE) {
    return ldr;
  }
  /* read strings */
  strs = readObjStrings(hdr.ostrs, hdr.sstrs, inFile, inPath);
  if (strs == NULL) {
    return LDERR_STR;
  }
  /* load segments */
  for (i = 0; i < hdr.nsegs; i++) {
    ldr = readObjSegment(&segRec, hdr.osegs, i, inFile, inPath);
    if (ldr != LDERR_NONE) {
      return ldr;
    }
    if (verbose) {
      showObjSegment(&segRec, strs);
    }
    if (segRec.attr & SEG_ATTR_A) {
      /* segment needs memory */
      dp = (void *) (segRec.addr + loadOff);
      if (segRec.attr & SEG_ATTR_P) {
        /* load from file */
        if (fseek(inFile, hdr.odata + segRec.offs, SEEK_SET) < 0) {
          return LDERR_DAT;
        }
        if (fread(dp, 1, segRec.size, inFile) != segRec.size) {
          return LDERR_DAT;
        }
      } else {
        /* load with zeros */
        memset(dp, 0, segRec.size);
      }
    }
  }
  /* set start address */
  *startAddr = hdr.entry;
  /* close input file */
  fclose(inFile);
  return LDERR_NONE;
}


int loadExecutable(char *execPath, unsigned int *startAddr) {
  int ldr;

  /* load executable */
  if (verbose) {
    printf("trying to load executable '%s'\n", execPath);
  }
  ldr = loadLinkUnit(execPath, EOF_X_MAGIC, 0, startAddr);
  return ldr;
}
