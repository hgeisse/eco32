/*
 * mkinit.c -- generate Verilog init file from C data
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ATT_FILE_NAME	"dspmem_att.init"
#define CHR_FILE_NAME	"dspmem_chr.init"


unsigned char data[][80] = {
  #include "screen"
};


int main(void) {
  FILE *outFile;
  int i, j;

  if (sizeof(data) != 30 * 80) {
    printf("Error: wrong size of data\n");
    exit(1);
  }
  outFile = fopen(ATT_FILE_NAME, "w");
  if (outFile == NULL) {
    printf("Error: cannot open file '%s'\n", ATT_FILE_NAME);
    exit(1);
  }
  for (i = 0; i < 32; i++) {
    for (j = 0; j < 128; j++) {
      if (j % 16 == 0) {
        fprintf(outFile, "@%04X ", i * 128 + j);
      }
      if (i >= 30 || j >= 80) {
        fprintf(outFile, "%02X ", 0x00);
      } else {
        fprintf(outFile, "%02X ", 0x07);
      }
      if (j % 16 == 15) {
        fprintf(outFile, "\n");
      }
    }
  }
  fclose(outFile);
  outFile = fopen(CHR_FILE_NAME, "w");
  if (outFile == NULL) {
    printf("Error: cannot open file '%s'\n", CHR_FILE_NAME);
    exit(1);
  }
  for (i = 0; i < 32; i++) {
    for (j = 0; j < 128; j++) {
      if (j % 16 == 0) {
        fprintf(outFile, "@%04X ", i * 128 + j);
      }
      if (i >= 30 || j >= 80) {
        fprintf(outFile, "%02X ", 0x00);
      } else {
        fprintf(outFile, "%02X ", data[i][j]);
      }
      if (j % 16 == 15) {
        fprintf(outFile, "\n");
      }
    }
  }
  fclose(outFile);
  return 0;
}
