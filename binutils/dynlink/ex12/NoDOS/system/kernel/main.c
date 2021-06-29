/*
 * main.c -- main program
 */


#include "stdarg.h"
#include "iolib.h"
#include "disk.h"


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
    printf("%6d  %s\n", dirEntry.byteSize, dirEntry.name);
  }
  closedir(dp);
}


/**************************************************************/


void cat(char *path) {
  FILE *fp;

  fp = fopen(path);
  if (fp == NULL) {
    printf("cannot open file '%s'\n", path);
    return;
  }
  fclose(fp);
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
    } else {
      printf("command '%s' not found\n", tokens[0]);
    }
  }
  printf("NoDOS halted.\n");
}
