/*
 * main.c -- main program
 */


#include "lib1.h"


int main(void) {
  int r11, r12, r21;

  r11 = (*pf11)();
  r12 = (*pf12)();
  r21 = (*pf21)();
  return r11 + r12 + r21;
}
