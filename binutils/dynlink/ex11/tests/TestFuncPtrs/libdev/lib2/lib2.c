/*
 * lib2.c -- library 2
 */


#include "lib2.h"


static void dummy(void) {
  int i, j;

  for (i = 0; i < 10; i++) {
    for (j = 0; j < 20; j++) {
      i++;
      j++;
    }
  }
}


int f21(void) {
  return 21;
}
