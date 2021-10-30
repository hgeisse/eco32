/*
 * xcmpl.c -- exchange args and complement boolean result for FP comparison
 */


#include <stdio.h>
#include <stdlib.h>


#define LINE_SIZE	200
#define ARG1		0
#define ARG2		9
#define RESULT		18


int main(int argc, char *argv[]) {
  char line[LINE_SIZE];
  char *s, *t;
  int i;
  char c;
  char *p;

  s = line + ARG1;
  t = line + ARG2;
  p = line + RESULT;
  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    for (i = 0; i < 8; i++) {
      c = s[i];
      s[i] = t[i];
      t[i] = c;
    }
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
