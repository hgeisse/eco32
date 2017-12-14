/*
 * mkdata.c -- generate data to be copied
 */


#include <stdio.h>
#include <stdlib.h>


#define DATA_SIZE	(0xC000 / 4)		/* in words */


int main(void) {
  int i;
  unsigned int r1, r2;

  srand(0x12345678);
  for (i = 0; i < DATA_SIZE; i++) {
    r1 = (rand() >> 4) & 0x0000FFFF;
    r2 = (rand() >> 4) & 0x0000FFFF;
    printf("\t.word\t0x%08X\n", (r1 << 16) | r2);
  }
  return 0;
}
