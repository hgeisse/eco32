/*
 * memchr.c -- find character in memory contents
 */


#include "string.h"


void *memchr(const void *cs, int c, size_t n) {
  unsigned char *dst;

  dst = (unsigned char *) cs;
  while (n-- != 0) {
    if (*dst == c) {
      return (void *) dst;
    }
    dst++;
  }
  return NULL;
}
