/*
 * main.c -- main program
 */


#include "lib1.h"


int main(void) {
  int r11, r12, r21;

  r11 = *pi11;
  r12 = *pi12;
  r21 = *pi21;
  return r11 + r12 + r21;
}
