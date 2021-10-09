/*
 * cmpl.c -- complement boolean result for FP comparison
 */


#include <stdio.h>
#include <stdlib.h>


#define LINE_SIZE	200
#define WHERE		18


int main(int argc, char *argv[]) {
  char line[LINE_SIZE];
  char *p;

  p = line + WHERE;
  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (*p == '0') {
      *p = '1';
    } else
    if (*p == '1') {
      *p = '0';
    } else {
      fprintf(stderr, "ERROR: ILLEGAL INPUT FOR cmpl()!\n");
      exit(1);
    }
    fputs(line, stdout);
  }
  return 0;
}
