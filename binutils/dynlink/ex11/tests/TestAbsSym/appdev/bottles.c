/*
 * bottles.c -- cheers! But look out for the value of 'sym'... ;-)
 */


#include "types.h"
#include "stdarg.h"
#include "iolib.h"


#define MAX_BEER	99


char *t = "bottle";
char *b = "of beer";
char *w = "on the wall";
char *m = "more";


extern unsigned char sym;


void main(void) {
  printf("No %s %ss %s %s, ", m, t, b, w);
  printf("no %s %ss %s.\n", m, t, b);
  printf("Go to the store and buy some %s, ", m);
  printf("%d %ss %s %s.\n", MAX_BEER, t, b, w);
  printf("\nsym = 0x%x\n\n", &sym);
}
