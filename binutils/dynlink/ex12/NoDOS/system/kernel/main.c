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
  char path[LINE_SIZE];
  int ldr;
  unsigned int startAddr;
  int retval;

  printf("\nWelcome to NoDOS, the No-Disk Operating System!\n\n");
  mountDisk();
  while (1) {
    getLine(PROMPT, line, LINE_SIZE);
    n = tokenize(line, tokens, MAX_TOKENS - 1);
    tokens[n] = NULL;
    if (n == 0) {
      continue;
    }
    /* load */
    strcpy(path, tokens[0]);
    ldr = loadExecutable(path, &startAddr);
    if (ldr == LDERR_FND && *tokens[0] != '/') {
      /* try alternative path */
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
