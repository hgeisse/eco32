/*
 * mod2.c
 */

#include "mod1.h"
#include "mod2.h"

int a2 = 3;
short b2 = 4;

int f2(void) {
  return a1 + h2();
}

short g2(void) {
  return b2 * g1();
}

char h2(void) {
  return c1[f1()];
}
