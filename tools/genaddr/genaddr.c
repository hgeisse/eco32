/*
 * genaddr.c -- address pattern generator
 */


#include <stdio.h>


#define START	0x3000
#define SMALL	0x10
#define	DIST	0x4000
#define	DIST1	0x0300
#define	DIST2	(0x1000 - 2 * DIST1)


int main(int argc, char *argv[]) {
  unsigned addr;
  int i, j;

  addr = START;
  for (i = 0; i < SMALL; i++) {
    for (j = 0; j < SMALL; j++) {
      printf("0x%08X\n", addr);
      addr++;
    }
    addr -= SMALL;
  }
  addr += DIST;
  for (i = 0; i < SMALL; i++) {
    for (j = 0; j < SMALL; j++) {
      printf("0x%08X\n", addr);
      addr++;
    }
    addr -= SMALL;
  }
  addr -= DIST;
  addr += DIST1;
  for (i = 0; i < SMALL * SMALL / 4; i++) {
    printf("0x%08X\n", addr);
    addr += DIST;
    printf("0x%08X\n", addr);
    addr -= DIST - DIST2;
    printf("0x%08X\n", addr);
    addr += DIST;
    printf("0x%08X\n", addr);
    addr -= DIST + DIST2;
  }
  addr -= DIST1;
  for (i = 0; i < SMALL / 2; i++) {
    for (j = 0; j < SMALL; j++) {
      printf("0x%08X\n", addr);
      addr += DIST;
      printf("0x%08X\n", addr);
      addr -= DIST - 1;
    }
  }
  return 0;
}
