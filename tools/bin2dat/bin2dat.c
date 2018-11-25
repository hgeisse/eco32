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
  int numBytes;
  int c;
  unsigned char lineData[16];
  int width;

  if (argc != 5) {
    printf("Usage: %s [-w|-h|-b] <input file> "
           "<output file> <memory size in KB>\n",
           argv[0]);
    exit(1);
  }
  if (strcmp(argv[1], "-w") == 0) {
    width = 4;
  } else
  if (strcmp(argv[1], "-h") == 0) {
    width = 2;
  } else
  if (strcmp(argv[1], "-b") == 0) {
    width = 1;
  } else {
    error("output width is not one of -w, -h, or -b");
  }
  infile = fopen(argv[2], "rb");
  if (infile == NULL) {
    error("cannot open input file %s", argv[2]);
  }
  outfile = fopen(argv[3], "wt");
  if (outfile == NULL) {
    error("cannot open output file %s", argv[3]);
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
  fprintf(outfile, "@0\n");
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
    switch (width) {
      case 1:
        fprintf(outfile, "%02X", lineData[0]);
        fprintf(outfile, "\n");
        fprintf(outfile, "%02X", lineData[1]);
        fprintf(outfile, "\n");
        fprintf(outfile, "%02X", lineData[2]);
        fprintf(outfile, "\n");
        fprintf(outfile, "%02X", lineData[3]);
        fprintf(outfile, "\n");
        break;
      case 2:
        fprintf(outfile, "%02X", lineData[0]);
        fprintf(outfile, "%02X", lineData[1]);
        fprintf(outfile, "\n");
        fprintf(outfile, "%02X", lineData[2]);
        fprintf(outfile, "%02X", lineData[3]);
        fprintf(outfile, "\n");
        break;
      case 4:
        fprintf(outfile, "%02X", lineData[0]);
        fprintf(outfile, "%02X", lineData[1]);
        fprintf(outfile, "%02X", lineData[2]);
        fprintf(outfile, "%02X", lineData[3]);
        fprintf(outfile, "\n");
        break;
    }
    totalWords++;
    if (c == EOF) {
      break;
    }
  }
  if (totalWords > memSize) {
    error("number of words exceeds memory size (%d words)", memSize);
  }
  while (totalWords < memSize) {
    switch (width) {
      case 1:
        fprintf(outfile, "00\n");
        fprintf(outfile, "00\n");
        fprintf(outfile, "00\n");
        fprintf(outfile, "00\n");
        break;
      case 2:
        fprintf(outfile, "0000\n");
        fprintf(outfile, "0000\n");
        break;
      case 4:
        fprintf(outfile, "00000000\n");
        break;
    }
    totalWords++;
  }
  fclose(infile);
  fclose(outfile);
  return 0;
}
