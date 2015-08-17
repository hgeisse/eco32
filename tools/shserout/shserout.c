/*
 * shserout.c -- show output of serial line in readable format
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LINE_SIZE	200


int main(int argc, char *argv[]) {
  FILE *inFile;
  char line[LINE_SIZE];
  char *p;
  unsigned int n;

  if (argc != 2) {
    printf("usage: %s <serial line output file>\n", argv[0]);
    exit(1);
  }
  inFile = fopen(argv[1], "r");
  if (inFile == NULL) {
    printf("error: cannot open input file '%s'\n", argv[1]);
    exit(1);
  }
  while (fgets(line, LINE_SIZE, inFile) != NULL) {
    p = strstr(line, "0x");
    if (p == NULL) {
      continue;
    }
    n = strtoul(p, NULL, 0);
    printf("%c", n);
  }
  fclose(inFile);
  return 0;
}
