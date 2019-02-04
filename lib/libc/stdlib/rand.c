/*
 * rand.c -- generate pseudo-random integers
 */


#include "stdlib.h"


static unsigned int next = 1;


int rand(void) {
  next = next * 1103515245 + 12345;
  return (next >> 16) & 0x7FFF;
}


void srand(unsigned int seed) {
  next = seed;
}
