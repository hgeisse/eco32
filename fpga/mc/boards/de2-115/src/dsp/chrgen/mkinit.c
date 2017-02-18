/*
 * mkinit.c -- generate Verilog init file from C data
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FILE_NAME	"chrgen_rom.init"


unsigned char data[] = {
  #include "font-8x16"
};


int main(void) {
  FILE *outFile;
  int i, j;

  if (sizeof(data) != 256 * 16) {
    printf("Error: wrong size of data\n");
    exit(1);
  }
  outFile = fopen(FILE_NAME, "w");
  if (outFile == NULL) {
    printf("Error: cannot open file '%s'\n", FILE_NAME);
    exit(1);
  }
  for (i = 0; i < 256 * 16; i++) {
    fprintf(outFile, "@%04X ", i * 8);
    for (j = 7; j >= 0; j--) {
      fprintf(outFile, "%c ", data[i] & (1 << j) ? '1' : '0');
    }
    fprintf(outFile, "\n");
  }
  fclose(outFile);
  return 0;
}
