/*
 * memset.c -- set memory contents
 */


#include "string.h"


void *memset(void *s, int c, size_t n) {
  unsigned char *dst;

  dst = (unsigned char *) s;
  while (n-- != 0) {
    *dst++ = c;
  }
  return s;
}
