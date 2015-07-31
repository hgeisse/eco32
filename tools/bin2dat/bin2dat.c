/*
 * bin2dat.c -- convert binary to simulation data
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#define WORDS_PER_KB	256


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
}


int main(int argc, char *argv[]) {
  FILE *infile;
  FILE *outfile;
  char *endptr;
  int memSize;
  int totalWords;
  int numBytes, i;
  int c;
  unsigned char lineData[16];

  if (argc != 4) {
    printf("Usage: %s <input file> <output file> <memory size in KB>\n",
           argv[0]);
    exit(1);
  }
  infile = fopen(argv[1], "rb");
  if (infile == NULL) {
    error("cannot open input file %s", argv[1]);
  }
  outfile = fopen(argv[2], "wt");
  if (outfile == NULL) {
    error("cannot open output file %s", argv[2]);
  }
  memSize = strtol(argv[3], &endptr, 0);
  if (*endptr != '\0') {
    error("cannot read memory size");
  }
  if (memSize <= 0) {
    error("illegal memory size %d", memSize);
  }
  memSize *= WORDS_PER_KB;
  totalWords = 0;
  while (1) {
    for (numBytes = 0; numBytes < 4; numBytes++) {
      c = fgetc(infile);
      if (c == EOF) {
        break;
      }
      lineData[numBytes] = c;
    }
    if (numBytes == 0) {
      break;
    }
    for (; numBytes < 4; numBytes++) {
      lineData[numBytes] = 0;
    }
    for (i = 0; i < numBytes; i++) {
      fprintf(outfile, "%02X", lineData[i]);
    }
    fprintf(outfile, "\n");
    totalWords++;
    if (c == EOF) {
      break;
    }
  }
  if (totalWords > memSize) {
    error("number of words exceeds memory size (%d words)", memSize);
  }
  while (totalWords < memSize) {
    fprintf(outfile, "00000000\n");
    totalWords++;
  }
  fclose(infile);
  fclose(outfile);
  return 0;
}
