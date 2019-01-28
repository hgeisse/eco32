/*
 * memcpy.c -- copy memory contents
 */


#include "string.h"


void *memcpy(void *s, const void *ct, size_t n) {
  unsigned char *dst;
  unsigned char *src;

  dst = (unsigned char *) s;
  src = (unsigned char *) ct;
  while (n-- != 0) {
    *dst++ = *src++;
  }
  return s;
}
