/*
 * bin2dat.c -- convert binary to simulation data
 *              (produce two 16-bit files)
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
  FILE *outfileHi;
  FILE *outfileLo;
  char *endptr;
  int memSize;
  int totalWords;
  int numBytes;
  int c;
  unsigned char lineData[16];

  if (argc != 5) {
    printf("Usage: %s\n", argv[0]);
    printf("           <input file> <high output file>\n");
    printf("           <low output file> <memory size in KB>\n");
    exit(1);
  }
  infile = fopen(argv[1], "rb");
  if (infile == NULL) {
    error("cannot open input file %s", argv[1]);
  }
  outfileHi = fopen(argv[2], "wt");
  if (outfileHi == NULL) {
    error("cannot open high output file %s", argv[2]);
  }
  outfileLo = fopen(argv[3], "wt");
  if (outfileLo == NULL) {
    error("cannot open low output file %s", argv[3]);
  }
  memSize = strtol(argv[4], &endptr, 0);
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
    fprintf(outfileHi, "%02X", lineData[0]);
    fprintf(outfileHi, "%02X", lineData[1]);
    fprintf(outfileHi, "\n");
    fprintf(outfileLo, "%02X", lineData[2]);
    fprintf(outfileLo, "%02X", lineData[3]);
    fprintf(outfileLo, "\n");
    totalWords++;
    if (c == EOF) {
      break;
    }
  }
  if (totalWords > memSize) {
    error("number of words exceeds memory size (%d words)", memSize);
  }
  while (totalWords < memSize) {
    fprintf(outfileHi, "0000\n");
    fprintf(outfileLo, "0000\n");
    totalWords++;
  }
  fclose(infile);
  fclose(outfileHi);
  fclose(outfileLo);
  return 0;
}
