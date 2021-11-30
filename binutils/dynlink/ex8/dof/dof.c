/*
 * dof.c -- dump object file
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eof.h"


/**************************************************************/


FILE *inFile;
ExecHeader execHeader;
SegmentRecord *segmentTable;
SymbolRecord *symbolTable;
RelocRecord *relocTable;


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
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


void dumpBytes(unsigned int totalSize) {
  unsigned int currSize;
  unsigned char line[16];
  int n, i;
  unsigned char c;

  currSize = 0;
  while (currSize < totalSize) {
    if (totalSize - currSize >= 16) {
      n = 16;
    } else {
      n = totalSize - currSize;
    }
    for (i = 0; i < n; i++) {
      line[i] = fgetc(inFile);
    }
    printf("%08X:  ", currSize);
    for (i = 0; i < 16; i++) {
      if (i < n) {
        c = line[i];
        printf("%02X", c);
      } else {
        printf("  ");
      }
      printf(" ");
    }
    printf("  ");
    for (i = 0; i < 16; i++) {
      if (i < n) {
        c = line[i];
        if (c >= 32 && c <= 126) {
          printf("%c", c);
        } else {
          printf(".");
        }
      } else {
        printf(" ");
      }
    }
    printf("\n");
    currSize += n;
  }
}


void dumpString(unsigned int offset) {
  long pos;
  int c;

  pos = ftell(inFile);
  if (fseek(inFile, execHeader.ostrs + offset, SEEK_SET) < 0) {
    error("cannot seek to string");
  }
  while (1) {
    c = fgetc(inFile);
    if (c == EOF) {
      error("unexpected end of file");
    }
    if (c == 0) {
      break;
    }
    fputc(c, stdout);
  }
  fseek(inFile, pos, SEEK_SET);
}


/**************************************************************/


void readHeader(void) {
  if (fseek(inFile, 0, SEEK_SET) < 0) {
    error("cannot seek to exec header");
  }
  if (fread(&execHeader, sizeof(ExecHeader), 1, inFile) != 1) {
    error("cannot read exec header");
  }
  conv4FromEcoToNative((unsigned char *) &execHeader.magic);
  conv4FromEcoToNative((unsigned char *) &execHeader.osegs);
  conv4FromEcoToNative((unsigned char *) &execHeader.nsegs);
  conv4FromEcoToNative((unsigned char *) &execHeader.osyms);
  conv4FromEcoToNative((unsigned char *) &execHeader.nsyms);
  conv4FromEcoToNative((unsigned char *) &execHeader.orels);
  conv4FromEcoToNative((unsigned char *) &execHeader.nrels);
  conv4FromEcoToNative((unsigned char *) &execHeader.odata);
  conv4FromEcoToNative((unsigned char *) &execHeader.sdata);
  conv4FromEcoToNative((unsigned char *) &execHeader.ostrs);
  conv4FromEcoToNative((unsigned char *) &execHeader.sstrs);
  conv4FromEcoToNative((unsigned char *) &execHeader.entry);
}


void dumpHeader(void) {
  if (execHeader.magic != EXEC_MAGIC) {
    error("wrong magic number in exec header");
  }
  printf("Header\n");
  printf("    magic number              : 0x%08X\n", execHeader.magic);
  printf("    offset of segment table   : 0x%08X\n", execHeader.osegs);
  printf("    number of segment entries : %10u\n", execHeader.nsegs);
  printf("    offset of symbol table    : 0x%08X\n", execHeader.osyms);
  printf("    number of symbol entries  : %10u\n", execHeader.nsyms);
  printf("    offset of reloc table     : 0x%08X\n", execHeader.orels);
  printf("    number of reloc entries   : %10u\n", execHeader.nrels);
  printf("    offset of segment data    : 0x%08X\n", execHeader.odata);
  printf("    size of segment data      : 0x%08X\n", execHeader.sdata);
  printf("    offset of string space    : 0x%08X\n", execHeader.ostrs);
  printf("    size of string space      : 0x%08X\n", execHeader.sstrs);
  printf("    entry point               : 0x%08X\n", execHeader.entry);
}


/**************************************************************/


void readSegmentTable(void) {
  int sn;

  segmentTable = memAlloc(execHeader.nsegs * sizeof(SegmentRecord));
  if (fseek(inFile, execHeader.osegs, SEEK_SET) < 0) {
    error("cannot seek to segment table");
  }
  for (sn = 0; sn < execHeader.nsegs; sn++) {
    if (fread(&segmentTable[sn], sizeof(SegmentRecord), 1, inFile) != 1) {
      error("cannot read segment record %d", sn);
    }
    conv4FromEcoToNative((unsigned char *) &segmentTable[sn].name);
    conv4FromEcoToNative((unsigned char *) &segmentTable[sn].offs);
    conv4FromEcoToNative((unsigned char *) &segmentTable[sn].addr);
    conv4FromEcoToNative((unsigned char *) &segmentTable[sn].size);
    conv4FromEcoToNative((unsigned char *) &segmentTable[sn].attr);
  }
}


void dumpSegmentTable(void) {
  int sn;

  printf("\nSegment Table\n");
  if (execHeader.nsegs == 0) {
    printf("<empty>\n");
    return;
  }
  for (sn = 0; sn < execHeader.nsegs; sn++) {
    printf("    %d:\n", sn);
    printf("        name = ");
    dumpString(segmentTable[sn].name);
    printf("\n");
    printf("        offs = 0x%08X\n", segmentTable[sn].offs);
    printf("        addr = 0x%08X\n", segmentTable[sn].addr);
    printf("        size = 0x%08X\n", segmentTable[sn].size);
    printf("        attr = ");
    if (segmentTable[sn].attr & SEG_ATTR_A) {
      printf("A");
    } else {
      printf("-");
    }
    if (segmentTable[sn].attr & SEG_ATTR_P) {
      printf("P");
    } else {
      printf("-");
    }
    if (segmentTable[sn].attr & SEG_ATTR_W) {
      printf("W");
    } else {
      printf("-");
    }
    if (segmentTable[sn].attr & SEG_ATTR_X) {
      printf("X");
    } else {
      printf("-");
    }
    printf("\n");
  }
}


/**************************************************************/


void readSymbolTable(void) {
  int sn;

  symbolTable = memAlloc(execHeader.nsyms * sizeof(SymbolRecord));
  if (fseek(inFile, execHeader.osyms, SEEK_SET) < 0) {
    error("cannot seek to symbol table");
  }
  for (sn = 0; sn < execHeader.nsyms; sn++) {
    if (fread(&symbolTable[sn], sizeof(SymbolRecord), 1, inFile) != 1) {
      error("cannot read symbol record %d", sn);
    }
    conv4FromEcoToNative((unsigned char *) &symbolTable[sn].name);
    conv4FromEcoToNative((unsigned char *) &symbolTable[sn].val);
    conv4FromEcoToNative((unsigned char *) &symbolTable[sn].seg);
    conv4FromEcoToNative((unsigned char *) &symbolTable[sn].attr);
  }
}


void dumpSymbolTable(void) {
  int sn;

  printf("\nSymbol Table\n");
  if (execHeader.nsyms == 0) {
    printf("<empty>\n");
    return;
  }
  for (sn = 0; sn < execHeader.nsyms; sn++) {
    printf("    %d:\n", sn);
    printf("        name = ");
    dumpString(symbolTable[sn].name);
    printf("\n");
    printf("        val  = 0x%08X\n", symbolTable[sn].val);
    printf("        seg  = %d\n", symbolTable[sn].seg);
    printf("        attr = ");
    if (symbolTable[sn].attr & SYM_ATTR_U) {
      printf("U");
    } else {
      printf("D");
    }
    printf("\n");
  }
}


/**************************************************************/


void readRelocTable(void) {
  int rn;

  relocTable = memAlloc(execHeader.nrels * sizeof(RelocRecord));
  if (fseek(inFile, execHeader.orels, SEEK_SET) < 0) {
    error("cannot seek to relocation table");
  }
  for (rn = 0; rn < execHeader.nrels; rn++) {
    if (fread(&relocTable[rn], sizeof(RelocRecord), 1, inFile) != 1) {
      error("cannot read relocation record %d", rn);
    }
    conv4FromEcoToNative((unsigned char *) &relocTable[rn].loc);
    conv4FromEcoToNative((unsigned char *) &relocTable[rn].seg);
    conv4FromEcoToNative((unsigned char *) &relocTable[rn].typ);
    conv4FromEcoToNative((unsigned char *) &relocTable[rn].ref);
    conv4FromEcoToNative((unsigned char *) &relocTable[rn].add);
  }
}


void dumpRelocTable(void) {
  int rn;

  printf("\nRelocation Table\n");
  if (execHeader.nrels == 0) {
    printf("<empty>\n");
    return;
  }
  for (rn = 0; rn < execHeader.nrels; rn++) {
    printf("    %d:\n", rn);
    printf("        loc  = 0x%08X\n", relocTable[rn].loc);
    printf("        seg  = %d\n", relocTable[rn].seg);
    printf("        typ  = ");
    switch (relocTable[rn].typ & ~RELOC_SYM) {
      case RELOC_H16:
        printf("H16");
        break;
      case RELOC_L16:
        printf("L16");
        break;
      case RELOC_R16:
        printf("R16");
        break;
      case RELOC_R26:
        printf("R26");
        break;
      case RELOC_W32:
        printf("W32");
        break;
      case RELOC_GOTADRH:
        printf("GOTADRH");
        break;
      case RELOC_GOTADRL:
        printf("GOTADRL");
        break;
      case RELOC_GOTOFFH:
        printf("GOTOFFH");
        break;
      case RELOC_GOTOFFL:
        printf("GOTOFFL");
        break;
      case RELOC_GOTPNTR:
        printf("GOTPNTR");
        break;
      case RELOC_LD_W32:
        printf("LD_W32");
        break;
      default:
        printf("\n");
        error("unknown relocation type 0x%08X", relocTable[rn].typ);
    }
    printf("\n");
    printf("        ref  = ");
    if (relocTable[rn].ref == -1) {
      printf("*nothing*");
    } else {
      printf("%s # %d",
             relocTable[rn].typ & RELOC_SYM ? "symbol" : "segment",
             relocTable[rn].ref);
    }
    printf("\n");
    printf("        add  = 0x%08X\n", relocTable[rn].add);
  }
}


/**************************************************************/


void dumpData(int sn) {
  printf("\nData of Segment %d\n", sn);
  if (!(segmentTable[sn].attr & SEG_ATTR_P)) {
    printf("<not present>\n");
    return;
  }
  if (segmentTable[sn].size == 0) {
    printf("<empty>\n");
    return;
  }
  if (fseek(inFile, execHeader.odata + segmentTable[sn].offs, SEEK_SET) < 0) {
    error("cannot seek to segment data");
  }
  dumpBytes(segmentTable[sn].size);
}


/**************************************************************/


void usage(char *myself) {
  printf("Usage: %s\n", myself);
  printf("         [-s]             dump symbol table\n");
  printf("         [-r]             dump relocations\n");
  printf("         [-d <n>]         dump data in segment <n>\n");
  printf("         [-a]             dump all\n");
  printf("         file             object file to be dumped\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  char *argp;
  char *inName;
  int optionSymbols;
  int optionRelocs;
  int optionData;
  int sn;
  char *endptr;

  inName = NULL;
  optionSymbols = 0;
  optionRelocs = 0;
  optionData = 0;
  for (i = 1; i < argc; i++) {
    argp = argv[i];
    if (*argp == '-') {
      argp++;
      switch (*argp) {
        case 's':
          optionSymbols = 1;
          break;
        case 'r':
          optionRelocs = 1;
          break;
        case 'd':
          optionData = 1;
          if (i == argc - 1) {
            error("option -d is missing a segment number");
          }
          sn = strtol(argv[++i], &endptr, 0);
          if (*endptr != '\0') {
            error("cannot read segment number in option -d");
          }
          break;
        case 'a':
          optionSymbols = 1;
          optionRelocs = 1;
          optionData = 2;
          break;
        default:
          usage(argv[0]);
      }
    } else {
      if (inName != NULL) {
        usage(argv[0]);
      }
      inName = argp;
    }
  }
  if (inName == NULL) {
    usage(argv[0]);
  }
  inFile = fopen(inName, "r");
  if (inFile == NULL) {
    error("cannot open input file '%s'", inName);
  }
  readHeader();
  dumpHeader();
  readSegmentTable();
  dumpSegmentTable();
  if (optionSymbols) {
    readSymbolTable();
    dumpSymbolTable();
  }
  if (optionRelocs) {
    readRelocTable();
    dumpRelocTable();
  }
  if (optionData) {
    if (optionData == 1) {
      if (sn < 0 || sn >= execHeader.nsegs) {
        error("option -d has illegal segment number %d", sn);
      }
      dumpData(sn);
    } else {
      for (sn = 0; sn < execHeader.nsegs; sn++) {
        dumpData(sn);
      }
    }
  }
  fclose(inFile);
  return 0;
}
