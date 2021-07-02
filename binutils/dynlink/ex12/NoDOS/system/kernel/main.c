/*
 * main.c -- main program
 */


#include "stdarg.h"
#include "iolib.h"
#include "disk.h"
#include "load.h"
#include "start.h"


#define PROMPT		"NoDOS> "
#define LINE_SIZE	200
#define MAX_TOKENS	50


/**************************************************************/


static int verbose = 1;


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


char *loadRes[] = {
  /*  0 */  "no error",
  /*  1 */  "not found",
  /*  2 */  "cannot read header",
  /*  3 */  "wrong magic number",
  /*  4 */  "cannot read strings",
  /*  5 */  "cannot read segments",
  /*  6 */  "cannot read data",
};

int maxRes = sizeof(loadRes) / sizeof(loadRes[0]);


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
  unsigned int startAddr;
  int retval;
  char path[LINE_SIZE];

  printf("\nWelcome to NoDOS, the No-Disk Operating System!\n\n");
  mountDisk();
  while (1) {
    getLine(PROMPT, line, LINE_SIZE);
    n = tokenize(line, tokens, MAX_TOKENS - 1);
    tokens[n] = NULL;
    if (n == 0) {
      continue;
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
    if (strcmp(tokens[0], "chkdsk") == 0) {
      chkdsk();
    } else {
      /* load */
      strcpy(path, tokens[0]);
      ldr = loadExecutable(path, &startAddr);
      if (ldr == LDERR_FND && *tokens[0] != '/') {
        /* try alternative */
        strcpy(path, "/bin/");
        strcat(path, tokens[0]);
        ldr = loadExecutable(path, &startAddr);
      }
      if (ldr == LDERR_NONE) {
        if (verbose) {
          printf("%s loaded, starting\n", tokens[0]);
        }
        /* run */
        retval = run(startAddr, n, tokens);
        /* clean up */
        if (verbose) {
          printf("%s stopped, return value = %d\n",
                 tokens[0], retval);
        }
      } else {
        printf("error: %s\n",
               ldr >= maxRes ? "unknown error" : loadRes[ldr]);
      }
    }
  }
}
