/*
 * load.c -- ECO32 loader
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eof.h"


/**************************************************************/


#define MAX_MEMSIZE		512	/* maximum memory size in MB */
#define DEFAULT_MEMSIZE		4	/* default memory size in MB */


/**************************************************************/


int verbose;

unsigned int memSize;
unsigned char *memory;

int loAddrSet;
unsigned int loAddr;
unsigned int hiAddr;


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


void *memAlloc(unsigned int size) {
  void *p;

  p = malloc(size);
  if (p == NULL) {
    error("out of memory");
  }
  return p;
}


void memFree(void *p) {
  if (p == NULL) {
    error("memFree() got NULL pointer");
  }
  free(p);
}


/**************************************************************/


unsigned int read4FromEco(unsigned char *p) {
  return (unsigned int) p[0] << 24 |
         (unsigned int) p[1] << 16 |
         (unsigned int) p[2] <<  8 |
         (unsigned int) p[3] <<  0;
}


void write4ToEco(unsigned char *p, unsigned int data) {
  p[0] = data >> 24;
  p[1] = data >> 16;
  p[2] = data >>  8;
  p[3] = data >>  0;
}


void conv4FromEcoToNative(unsigned char *p) {
  unsigned int data;

  data = read4FromEco(p);
  * (unsigned int *) p = data;
}


void conv4FromNativeToEco(unsigned char *p) {
  unsigned int data;

  data = * (unsigned int *) p;
  write4ToEco(p, data);
}


/**************************************************************/


typedef struct linkUnit {
  char *name;
  char *strs;
} LinkUnit;


LinkUnit *newLinkUnit(char *name) {
  LinkUnit *linkUnit;

  linkUnit = memAlloc(sizeof(LinkUnit));
  linkUnit->name = name;
  linkUnit->strs = NULL;
  return linkUnit;
}


/**************************************************************/


void readObjHeader(EofHeader *hdr,
                   FILE *inFile, char *inPath,
                   unsigned int magic) {
  if (fseek(inFile, 0, SEEK_SET) < 0) {
    error("cannot seek to header in input file '%s'", inPath);
  }
  if (fread(hdr, sizeof(EofHeader), 1, inFile) != 1) {
    error("cannot read header in input file '%s'", inPath);
  }
  conv4FromEcoToNative((unsigned char *) &hdr->magic);
  conv4FromEcoToNative((unsigned char *) &hdr->osegs);
  conv4FromEcoToNative((unsigned char *) &hdr->nsegs);
  conv4FromEcoToNative((unsigned char *) &hdr->osyms);
  conv4FromEcoToNative((unsigned char *) &hdr->nsyms);
  conv4FromEcoToNative((unsigned char *) &hdr->orels);
  conv4FromEcoToNative((unsigned char *) &hdr->nrels);
  conv4FromEcoToNative((unsigned char *) &hdr->odata);
  conv4FromEcoToNative((unsigned char *) &hdr->sdata);
  conv4FromEcoToNative((unsigned char *) &hdr->ostrs);
  conv4FromEcoToNative((unsigned char *) &hdr->sstrs);
  conv4FromEcoToNative((unsigned char *) &hdr->entry);
  conv4FromEcoToNative((unsigned char *) &hdr->olibs);
  conv4FromEcoToNative((unsigned char *) &hdr->nlibs);
  if (hdr->magic != magic) {
    error("wrong magic number in input file '%s'", inPath);
  }
}


char *readObjStrings(unsigned int ostrs, unsigned int sstrs,
                     FILE *inFile, char *inPath) {
  char *strs;

  strs = memAlloc(sstrs);
  if (fseek(inFile, ostrs, SEEK_SET) < 0) {
    error("cannot seek to strings in input file '%s'", inPath);
  }
  if (fread(strs, 1, sstrs, inFile) != sstrs) {
    error("cannot read strings in input file '%s'", inPath);
  }
  return strs;
}


void readObjSegment(SegmentRecord *seg,
                    unsigned int osegs, int segno,
                    FILE *inFile, char *inPath) {
  if (fseek(inFile, osegs + segno * sizeof(SegmentRecord), SEEK_SET) < 0) {
    error("cannot seek to segment record %d in input file '%s'",
          segno, inPath);
  }
  if (fread(seg, sizeof(SegmentRecord), 1, inFile) != 1) {
    error("cannot read segment record %d in input file '%s'",
          segno, inPath);
  }
  conv4FromEcoToNative((unsigned char *) &seg->name);
  conv4FromEcoToNative((unsigned char *) &seg->offs);
  conv4FromEcoToNative((unsigned char *) &seg->addr);
  conv4FromEcoToNative((unsigned char *) &seg->size);
  conv4FromEcoToNative((unsigned char *) &seg->attr);
}


void showObjSegment(SegmentRecord *seg, char *strs) {
  printf("    loading segment '%s'\n", strs + seg->name);
  printf("        offset  : 0x%08X\n", seg->offs);
  printf("        address : 0x%08X\n", seg->addr);
  printf("        size    : 0x%08X\n", seg->size);
  printf("        attr    : ");
  if (seg->attr & SEG_ATTR_A) {
    printf("A");
  } else {
    printf("-");
  }
  if (seg->attr & SEG_ATTR_P) {
    printf("P");
  } else {
    printf("-");
  }
  if (seg->attr & SEG_ATTR_W) {
    printf("W");
  } else {
    printf("-");
  }
  if (seg->attr & SEG_ATTR_X) {
    printf("X");
  } else {
    printf("-");
  }
  printf("\n");
}


void readObjReloc(RelocRecord *rel,
                  unsigned int orels, int relno,
                  FILE *inFile, char *inPath) {
  if (fseek(inFile, orels + relno * sizeof(RelocRecord), SEEK_SET) < 0) {
    error("cannot seek to relocation record %d in input file '%s'",
          relno, inPath);
  }
  if (fread(rel, sizeof(RelocRecord), 1, inFile) != 1) {
    error("cannot read relocation record %d in input file '%s'",
          relno, inPath);
  }
  conv4FromEcoToNative((unsigned char *) &rel->loc);
  conv4FromEcoToNative((unsigned char *) &rel->seg);
  conv4FromEcoToNative((unsigned char *) &rel->typ);
  conv4FromEcoToNative((unsigned char *) &rel->ref);
  conv4FromEcoToNative((unsigned char *) &rel->add);
}


char *relocMethod[] = {
  /*  0 */  "H16",
  /*  1 */  "L16",
  /*  2 */  "R16",
  /*  3 */  "R26",
  /*  4 */  "W32",
  /*  5 */  "GA_H16",
  /*  6 */  "GA_L16",
  /*  7 */  "GR_H16",
  /*  8 */  "GR_L16",
  /*  9 */  "GP_L16",
  /* 10 */  "ER_W32",
};


void showObjReloc(RelocRecord *rel, int rn) {
  printf("    applying relocation %u\n", rn);
  printf("        loc  = 0x%08X\n", rel->loc);
  printf("        seg  = ");
  if (rel->seg == -1) {
    printf("*none*");
  } else {
    printf("%d", rel->seg);
  }
  printf("\n");
  printf("        typ  = %s\n", relocMethod[rel->typ & ~RELOC_SYM]);
  printf("        ref  = ");
  if (rel->ref == -1) {
    printf("*none*");
  } else {
    printf("%s # %d",
           rel->typ & RELOC_SYM ? "symbol" : "segment", rel->ref);
  }
  printf("\n");
  printf("        add  = 0x%08X\n", rel->add);
}


LinkUnit *loadLinkUnit(char *name, unsigned int magic,
                       FILE *inFile, char *inPath,
                       unsigned int ldOff) {
  LinkUnit *linkUnit;
  EofHeader hdr;
  char *strs;
  int i;
  SegmentRecord seg;
  unsigned char *dp;
  RelocRecord rel;
  unsigned int word;

  linkUnit = newLinkUnit(name);
  /* read header */
  readObjHeader(&hdr, inFile, inPath, magic);
  /* read strings */
  strs = readObjStrings(hdr.ostrs, hdr.sstrs, inFile, inPath);
  linkUnit->strs = strs;
  /* load segments */
  for (i = 0; i < hdr.nsegs; i++) {
    readObjSegment(&seg, hdr.osegs, i, inFile, inPath);
    if (verbose) {
      showObjSegment(&seg, strs);
    }
    if (seg.attr & SEG_ATTR_A) {
      /* segment needs memory */
      if (!loAddrSet) {
        loAddrSet = 1;
        loAddr = seg.addr + ldOff;
      }
      hiAddr = seg.addr + ldOff + seg.size;
      dp = memory + (seg.addr + ldOff - loAddr);
      if (seg.attr & SEG_ATTR_P) {
        /* load from file */
        if (fseek(inFile, hdr.odata + seg.offs, SEEK_SET) < 0) {
          error("cannot seek to data of segment '%s' in file '%s'",
                strs + seg.name, inPath);
        }
        if (fread(dp, 1, seg.size, inFile) != seg.size) {
          error("cannot read data of segment '%s' in file '%s'",
                strs + seg.name, inPath);
        }
      } else {
        /* load with zeros */
        memset(dp, 0, seg.size);
      }
    }
  }
  /* apply relocations */
  for (i = 0; i < hdr.nrels; i++) {
    readObjReloc(&rel, hdr.orels, i, inFile, inPath);
    if (verbose) {
      showObjReloc(&rel, i);
    }
    if (rel.seg != -1 ||
        rel.typ != RELOC_ER_W32 ||
        rel.ref != -1 ||
        rel.add != 0) {
      error("illegal relocation %d", i);
    }
    dp = memory + (rel.loc + ldOff - loAddr);
    word = read4FromEco(dp);
    word += ldOff;
    write4ToEco(dp, word);
  }
  return linkUnit;
}


/**************************************************************/


void loadExecutable(char *execPath, unsigned int ldOff) {
  FILE *execFile;
  char *baseName;

  if (verbose) {
    printf("loading from executable file '%s'\n", execPath);
    if (ldOff != 0) {
      printf("(with load offset 0x%08X)\n", ldOff);
    }
  }
  execFile = fopen(execPath, "r");
  if (execFile == NULL) {
    error("cannot open executable file '%s'", execPath);
  }
  baseName = execPath + strlen(execPath);
  while (baseName != execPath && *baseName != '/') {
    baseName--;
  }
  if (*baseName == '/') {
    baseName++;
  }
  loadLinkUnit(baseName, EOF_X_MAGIC, execFile, execPath, ldOff);
  fclose(execFile);
}


/**************************************************************/


void writeBinary(char *binaryPath) {
  FILE *binaryFile;
  unsigned int size;

  if (verbose) {
    printf("writing to binary file '%s'\n", binaryPath);
  }
  binaryFile = fopen(binaryPath, "w");
  if (binaryFile == NULL) {
    error("cannot open binary file '%s'", binaryPath);
  }
  if (loAddrSet != 0 && loAddr < hiAddr) {
    size = hiAddr - loAddr;
    if (fwrite(memory, 1, size, binaryFile) != size) {
      error("cannot write to binary file '%s'", binaryPath);
    }
  }
  fclose(binaryFile);
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s [options] <executable> <binary>\n", myself);
  printf("valid options are:\n");
  printf("    -v             verbose\n");
  printf("    -l <n>         load with n bytes offset\n");
  printf("    -m <n>         set maximum memory size to <n> MB\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  unsigned int ldOff;
  unsigned int msize;
  char *execFileName;
  char *binFileName;
  int i;
  char *argp;
  char *endp;

  verbose = 0;
  ldOff = 0;
  msize = DEFAULT_MEMSIZE;
  execFileName = NULL;
  binFileName = NULL;
  for (i = 1; i < argc; i++) {
    argp = argv[i];
    if (*argp == '-') {
      /* option */
      if (strcmp(argp, "-v") == 0) {
        verbose = 1;
      } else
      if (strcmp(argp, "-l") == 0) {
        if (i == argc - 1) {
          usage(argv[0]);
        }
        ldOff = strtoul(argv[++i], &endp, 0);
        if (*endp != '\0') {
          error("illegal load offset");
        }
      } else
      if (strcmp(argp, "-m") == 0) {
        if (i == argc - 1) {
          usage(argv[0]);
        }
        msize = strtoul(argv[++i], &endp, 0);
        if (*endp != '\0' ||
            msize <= 0 ||
            msize > MAX_MEMSIZE) {
          error("illegal memory size");
        }
      } else {
        usage(argv[0]);
      }
    } else {
      /* file */
      if (execFileName == NULL) {
        execFileName = argp;
      } else
      if (binFileName == NULL) {
        binFileName = argp;
      } else{
        usage(argv[0]);
      }
    }
  }
  if (execFileName == NULL) {
    error("no executable file specified");
  }
  if (binFileName == NULL) {
    error("no binary file specified");
  }
  if (verbose) {
    printf("initializing %u MB of memory\n", msize);
  }
  memSize = msize << 20;
  memory = memAlloc(memSize);
  for (i = 0; i < memSize; i++) {
    memory[i] = rand();
  }
  loAddrSet = 0;
  loadExecutable(execFileName, ldOff);
  writeBinary(binFileName);
  return 0;
}
