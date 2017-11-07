/*
 * mod1.c
 */

#include "mod1.h"
#include "mod2.h"

int a1 = 1;
short b1 = 2;
char c1[4] = { 'a', 'b', 'c', 'd' };

int f1(void) {
  return a1;
}

short g1(void) {
  return b1 + a2;
}

int main(void) {
  return f2() + g2();
}
