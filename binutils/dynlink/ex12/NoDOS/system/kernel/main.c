/*
 * main.c -- main program
 */


#include "stdarg.h"
#include "iolib.h"
#include "disk.h"
#include "eof.h"


#define PROMPT		"NoDOS> "
#define LINE_SIZE	200
#define MAX_TOKENS	20


/**************************************************************/


void ls(void) {
  DIR *dp;
  DirEntry dirEntry;

  dp = opendir();
  while (1) {
    if (readdir(&dirEntry, dp) < 0) {
      break;
    }
    printf("%8d  %s\n", dirEntry.byteSize, dirEntry.name);
  }
  closedir(dp);
}


/**************************************************************/


void cat(char *path) {
  FILE *fp;
  char c;

  fp = fopen(path);
  if (fp == NULL) {
    printf("cannot open file '%s'\n", path);
    return;
  }
  while (fread(&c, 1, 1, fp) == 1) {
    putchar(c);
  }
  fclose(fp);
}


/**************************************************************/


#define NUM_TESTS	20
#define MAX_WHERE	(12 * 4096)
#define MAX_COUNT	(4 * 4096)


unsigned char ref[] = {
  #include "../disk/randbytes.txt"
};


unsigned int randomNumber = 11111111;


unsigned int getRandomNumber(void) {
  randomNumber = randomNumber * (unsigned) 1103515245 + (unsigned) 12345;
  return randomNumber;
}


int check(unsigned int where, unsigned int count, FILE *fp) {
  unsigned char buf[MAX_COUNT];
  int i;

  printf("reading %d bytes from offset %d\n", count, where);
  if (fseek(fp, where, SEEK_SET) < 0) {
    printf("cannot set position in file '/data/randbytes'\n");
    return 1;
  }
  if (fread(buf, 1, count, fp) != count) {
    printf("cannot read file '/data/randbytes'\n");
    return 1;
  }
  for (i = 0; i < count; i++) {
    if (buf[i] != ref[where + i]) {
      printf("data read not equal to reference\n");
      return 1;
    }
  }
  return 0;
}


void chkdsk(void) {
  FILE *fp;
  int i;
  unsigned int where, count;

  fp = fopen("/data/randbytes");
  if (fp == NULL) {
    printf("cannot open file '/data/randbytes'\n");
    return;
  }
  if (check(0, 4095, fp)) {
    fclose(fp);
    return;
  }
  if (check(0, 4096, fp)) {
    fclose(fp);
    return;
  }
  if (check(0, 4097, fp)) {
    fclose(fp);
    return;
  }
  if (check(4 * 4096 + 4095, 4095, fp)) {
    fclose(fp);
    return;
  }
  if (check(4 * 4096 + 4096, 4096, fp)) {
    fclose(fp);
    return;
  }
  if (check(4 * 4096 + 4097, 4097, fp)) {
    fclose(fp);
    return;
  }
  if (check(7 * 4096 + 4095, 2 * 4096 + 4095, fp)) {
    fclose(fp);
    return;
  }
  if (check(7 * 4096 + 4096, 2 * 4096 + 4096, fp)) {
    fclose(fp);
    return;
  }
  if (check(7 * 4096 + 4097, 2 * 4096 + 4097, fp)) {
    fclose(fp);
    return;
  }
  for (i = 0; i < NUM_TESTS; i++) {
    where = (getRandomNumber() >> 5) % MAX_WHERE;
    do {
      count = (getRandomNumber() >> 5) % MAX_COUNT;
    } while (count == 0);
    if (check(where, count, fp)) {
      fclose(fp);
      return;
    }
  }
  fclose(fp);
  printf("no errors\n");
}


/**************************************************************/


#define LDERR_NONE	0	/* no error */
#define LDERR_FND	1	/* not found */
#define LDERR_HDR	2	/* cannot read header */
#define LDERR_MGC	3	/* wrong magic number */
#define LDERR_STR	4	/* cannot read strings */
#define LDERR_SEG	5	/* cannot read segments */


char *loadRes[] = {
  /*  0 */  "no error",
  /*  1 */  "not found",
  /*  2 */  "cannot read header",
  /*  3 */  "wrong magic number",
  /*  4 */  "cannot read strings",
  /*  5 */  "cannot read segments",
};

int maxRes = sizeof(loadRes) / sizeof(loadRes[0]);


int verbose = 1;


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
                 unsigned int loadOff) {
  FILE *inFile;
  int ldr;
  EofHeader hdr;
  char *strs;
  int i;
  SegmentRecord segRec;

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
      if (segRec.attr & SEG_ATTR_P) {
        /* load from file */
      } else {
        /* load with zeros */
      }
    }
  }
  /* close input file */
  fclose(inFile);
  return LDERR_NONE;
}


int loadExecutable(char *execPath) {
  int ldr;

  /* load executable */
  if (verbose) {
    printf("loading from executable '%s'\n", execPath);
  }
  ldr = loadLinkUnit(execPath, EOF_X_MAGIC, 0);
  return ldr;
}


/**************************************************************/


int tokenize(char *line, char *tokens[], int maxTokens) {
  int n;
  char *p;

  n = 0;
  p = strtok(line, " \t\n");
  while (p != NULL) {
    if (n < maxTokens) {
      tokens[n] = p;
      n++;
    }
    p = strtok(NULL, " \t\n");
  }
  return n;
}


void main(void) {
  char line[LINE_SIZE];
  char *tokens[MAX_TOKENS];
  int n;
  int ldr;

  printf("Welcome to NoDOS, the No-Disk Operating System!\n\n");
  mountDisk();
  while (1) {
    getLine(PROMPT, line, LINE_SIZE);
    n = tokenize(line, tokens, MAX_TOKENS);
    if (n == 0) {
      continue;
    }
    if (strcmp(tokens[0], "halt") == 0) {
      break;
    }
    if (strcmp(tokens[0], "ls") == 0) {
      ls();
    } else
    if (strcmp(tokens[0], "cat") == 0) {
      if (n < 2) {
        printf("missing file name\n");
      } else {
        cat(tokens[1]);
      }
    } else
    if (strcmp(tokens[0], "help") == 0) {
      cat("/txt/help.txt");
    } else
    if (strcmp(tokens[0], "chkdsk") == 0) {
      chkdsk();
    } else {
      ldr = loadExecutable(tokens[0]);
      if (ldr == LDERR_NONE) {
        printf("%s loaded\n", tokens[0]);
      } else {
        printf("error: %s\n",
               ldr >= maxRes ? "unknown error" : loadRes[ldr]);
      }
    }
  }
  printf("NoDOS halted.\n");
}
