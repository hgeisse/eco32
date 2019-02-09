/*
 * string.c -- test string functions
 */


#include <stdio.h>
#include <string.h>
#include "../include/string.h"


void stop(void) {
  int i;

  for (i = 0; i < 1000000; i++) ;
  *(unsigned int *)0xFF100000 = 0;
}


int main(void) {
  stop();
  return 0;
}
