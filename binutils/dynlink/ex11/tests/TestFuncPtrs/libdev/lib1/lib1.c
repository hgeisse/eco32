/*
 * lib1.c -- library 1
 */


#include "lib2.h"
#include "lib1.h"


static void dummy(void) {
  int i;

  for (i = 0; i < 10; i++) {
    i++;
  }
}


static int f11(void) {
  return 11;
}


int f12(void) {
  return 12;
}


static int dummyData[] = {
  1, 2, 3, 4, 5, 6, 7, 8
};


int (*pf11)(void) = f11;
int (*pf12)(void) = f12;
int (*pf21)(void) = f21;
