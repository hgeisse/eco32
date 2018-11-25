/*
 * gen.c -- generate a file with 64 KB (16 KW) random values
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
  int i;
  unsigned char b0, b1, b2, b3;
  unsigned int w;

  printf(";\n");
  printf("; romrand.s -- 64 KB (16 KW) random values\n");
  printf(";\n");
  printf("\n");
  for (i = 0; i < 16384; i++) {
    if ((i & 3) == 0) {
      printf("\t.word\t");
    }
    b0 = rand();
    b1 = rand();
    b2 = rand();
    b3 = rand();
    w = ((unsigned int) b0 << 24) |
        ((unsigned int) b1 << 16) |
        ((unsigned int) b2 <<  8) |
        ((unsigned int) b3 <<  0);
    printf("0x%08X", w);
    if ((i & 3) == 3) {
      printf("\n");
    } else {
      printf(", ");
    }
  }
  return 0;
}
