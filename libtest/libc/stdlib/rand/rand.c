/*
 * rand.c -- test random number generator
 */


#include <stdio.h>
#include <stdlib.h>


unsigned int refDat[] = {
  #include "ref.dat"
};


int main(void) {
  int i;
  int next;
  int diff;

  srand(123456);
  diff = 0;
  for (i = 0; i < sizeof(refDat)/sizeof(refDat[0]); i++) {
    next = rand();
    if (next != refDat[i]) {
      diff++;
    }
  }
  printf("%d random numbers generated\n", i);
  printf("%d differences\n", diff);
  return 0;
}
