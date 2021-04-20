/*
 * mylib.c -- example library
 */


#include "mylib.h"


void putchar(char c) {
  unsigned int *base;

  if (c == '\n') {
    putchar('\r');
  }
  base = (unsigned int *) 0xF0300000;
  while ((*(base + 2) & 1) == 0) ;
  *(base + 3) = c;
}


void puts(char *s) {
  char c;

  while ((c = *s++) != '\0') {
    putchar(c);
  }
  putchar('\n');
}


void myfunc(void) {
  puts("myfunc() in mylib executing");
}
