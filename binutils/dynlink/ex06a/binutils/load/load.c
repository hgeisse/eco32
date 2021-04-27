/*
 * load.c -- ECO32 loader
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eof.h"


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
}


/**************************************************************/


unsigned int read4(unsigned char *p) {
  return (unsigned int) p[0] << 24 |
         (unsigned int) p[1] << 16 |
         (unsigned int) p[2] <<  8 |
         (unsigned int) p[3] <<  0;
}


/**************************************************************/


#define LDERR_NONE	0	/* no error */
#define LDERR_SFH	1	/* cannot seek to file header */
#define LDERR_RFH	2	/* cannot read file header */
#define LDERR_WMN	3	/* wrong magic number in file header */
#define LDERR_SEG	4	/* no segments */
#define LDERR_MEM	5	/* no memory */
#define LDERR_SSE	6	/* cannot seek to segment entry */
#define LDERR_RSE	7	/* cannot read segment entry */
#define LDERR_SSD	8	/* cannot seek to segment data */
#define LDERR_RSD	9	/* cannot read segment data */
#define LDERR_WBF	10	/* cannot write binary file */
#define LDERR_SNO	11	/* segments not ordered, cannot fill gap */


typedef struct {
  unsigned int vaddr;
  unsigned int size;
  unsigned int attr;
  unsigned char *data;
} SegmentInfo;


int segCmp(const void *entry1, const void *entry2) {
  unsigned int vaddr1;
  unsigned int vaddr2;

  vaddr1 = ((SegmentInfo *) entry1)->vaddr;
  vaddr2 = ((SegmentInfo *) entry2)->vaddr;
  if (vaddr1 < vaddr2) {
    return -1;
  }
  if (vaddr1 > vaddr2) {
    return 1;
  }
  return 0;
}


int loadObj(FILE *inFile, FILE *outFile,
            int sort, int fill, int clear, int verbose) {
  EofHeader fileHeader;
  unsigned int osegs;
  unsigned int nsegs;
  unsigned int odata;
  unsigned int entry;
  SegmentInfo *allSegs;
  int numSegs;
  int i;
  SegmentRecord segRec;
  unsigned int offs;
  unsigned int addr;
  unsigned int size;
  unsigned int attr;
  unsigned char *data;
  unsigned int vaddr;
  int j;

  /* read and inspect file header */
  if (fseek(inFile, 0, SEEK_SET) < 0) {
    return LDERR_SFH;
  }
  if (fread(&fileHeader, sizeof(EofHeader), 1, inFile) != 1) {
    return LDERR_RFH;
  }
  if (read4((unsigned char *) &fileHeader.magic) != EOF_MAGIC) {
    return LDERR_WMN;
  }
  osegs = read4((unsigned char *) &fileHeader.osegs);
  nsegs = read4((unsigned char *) &fileHeader.nsegs);
  odata = read4((unsigned char *) &fileHeader.odata);
  entry = read4((unsigned char *) &fileHeader.entry);
  if (verbose) {
    printf("info: entry point      : 0x%08X\n", entry);
    printf("      num segments     : %d\n", nsegs);
    printf("      segment table at : 0x%08X bytes file offset\n", osegs);
    printf("      segment data at  : 0x%08X bytes file offset\n", odata);
  }
  if (nsegs == 0) {
    return LDERR_SEG;
  }
  allSegs = malloc(nsegs * sizeof(SegmentInfo));
  if (allSegs == NULL) {
    return LDERR_MEM;
  }
  /* read segments */
  numSegs = 0;
  for (i = 0; i < nsegs; i++) {
    if (verbose) {
      printf("processing segment entry %d\n", i);
    }
    if (fseek(inFile, osegs + i * sizeof(SegmentRecord), SEEK_SET) < 0) {
      return LDERR_SSE;
    }
    if (fread(&segRec, sizeof(SegmentRecord), 1, inFile) != 1) {
      return LDERR_RSE;
    }
    offs = read4((unsigned char *) &segRec.offs);
    addr = read4((unsigned char *) &segRec.addr);
    size = read4((unsigned char *) &segRec.size);
    attr = read4((unsigned char *) &segRec.attr);
    if (verbose) {
      printf("    offset  : 0x%08X\n", offs);
      printf("    address : 0x%08X\n", addr);
      printf("    size    : 0x%08X\n", size);
      printf("    attr    : 0x%08X\n", attr);
    }
    if (attr & SEG_ATTR_P) {
      data = malloc(size);
      if (data == NULL) {
        return LDERR_MEM;
      }
      if (fseek(inFile, odata + offs, SEEK_SET) < 0) {
        return LDERR_SSD;
      }
      if (fread(data, 1, size, inFile) != size) {
        return LDERR_RSD;
      }
      if (verbose) {
        printf("    segment of 0x%08X bytes read, vaddr = 0x%08X\n",
               size, addr);
      }
    } else {
      data = NULL;
    }
    allSegs[i].vaddr = addr;
    allSegs[i].size = size;
    allSegs[i].attr = attr;
    allSegs[i].data = data;
    numSegs++;
  }
  /* possibly sort segments */
  if (sort) {
    qsort(allSegs, numSegs, sizeof(SegmentInfo), segCmp);
  }
  /* write segments */
  vaddr = allSegs[0].vaddr;
  for (i = 0; i < numSegs; i++) {
    if (!(allSegs[i].attr & SEG_ATTR_A)) {
      continue;
    }
    if (allSegs[i].size == 0) {
      continue;
    }
    if (fill) {
      /* fill inter-segment gap */
      if (vaddr > allSegs[i].vaddr) {
        /* segments not ordered, cannot fill gap */
        return LDERR_SNO;
      }
      while (vaddr < allSegs[i].vaddr) {
        if (fputc(0, outFile) == EOF) {
          return LDERR_WBF;
        }
        vaddr++;
      }
    } else {
      /* do not fill inter-segment gap */
      vaddr = allSegs[i].vaddr;
    }
    size = allSegs[i].size;
    if (allSegs[i].attr & SEG_ATTR_P) {
      if (fwrite(allSegs[i].data, 1, size, outFile) != size) {
        return LDERR_WBF;
      }
    } else {
      if (clear) {
        /* clear non-present segment */
        for (j = 0; j < size; j++) {
          if (fputc(0, outFile) == EOF) {
            return LDERR_WBF;
          }
        }
      }
    }
    vaddr += size;
  }
  return LDERR_NONE;
}


/**************************************************************/


char *loadResult[] = {
  /*  0 */  "no error",
  /*  1 */  "cannot seek to file header",
  /*  2 */  "cannot read file header",
  /*  3 */  "wrong magic number in file header",
  /*  4 */  "no segments",
  /*  5 */  "no memory",
  /*  6 */  "cannot seek to segment entry",
  /*  7 */  "cannot read segment entry",
  /*  8 */  "cannot seek to segment data",
  /*  9 */  "cannot read segment data",
  /* 10 */  "cannot write binary file",
  /* 11 */  "segments not ordered, cannot fill gap",
};

int maxResults = sizeof(loadResult) / sizeof(loadResult[0]);


void usage(char *myself) {
  printf("usage: %s [-p] [-v] <object file> <binary file>\n", myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  int pack;
  int verbose;
  char *inName;
  char *outName;
  int i;
  FILE *inFile;
  FILE *outFile;
  int result;

  pack = 0;
  verbose = 0;
  inName = NULL;
  outName = NULL;
  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {
      /* option */
      if (strcmp(argv[i], "-p") == 0) {
        pack = 1;
      } else
      if (strcmp(argv[i], "-v") == 0) {
        verbose = 1;
      } else {
        usage(argv[0]);
      }
    } else {
      /* file */
      if (inName == NULL) {
        inName = argv[i];
      } else
      if (outName == NULL) {
        outName = argv[i];
      } else {
        error("more than two files specified");
      }
    }
  }
  if (inName == NULL) {
    error("no object file specified");
  }
  if (outName == NULL) {
    error("no binary file specified");
  }
  inFile = fopen(inName, "rb");
  if (inFile == NULL) {
    error("cannot open object file '%s'", inName);
  }
  outFile = fopen(outName, "wb");
  if (outFile == NULL) {
    error("cannot open binary file '%s'", outName);
  }
  result = loadObj(inFile, outFile, 0, !pack, !pack, verbose);
  if (verbose || result != 0) {
    printf("%s: %s\n",
           result == 0 ? "result" : "error",
           result >= maxResults ? "unknown error number" :
                                  loadResult[result]);
  }
  fclose(inFile);
  fclose(outFile);
  return result;
}
