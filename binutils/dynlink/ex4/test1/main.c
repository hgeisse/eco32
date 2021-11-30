/*
 * main.c -- main program
 */

#include "common.h"

char *str = "Hello, world!\n";

int main(int argc, char *argv[]) {
  int sum;

  sum = f();
  /*
   * sum should be 0x00000493
   * (contents of $2 when program finally loops in startup)
   */
  return sum;
}
