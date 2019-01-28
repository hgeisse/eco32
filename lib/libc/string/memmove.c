/*
 * memmove.c -- move memory contents (src and dst may overlap)
 */


#include "string.h"


void *memmove(void *s, const void *ct, size_t n) {
  unsigned char *dst;
  unsigned char *src;

  if (dst < src) {
    /* direction: up */
    dst = (unsigned char *) s;
    src = (unsigned char *) ct;
    while (n-- != 0) {
      *dst++ = *src++;
    }
  } else {
    /* direction: down */
    dst = (unsigned char *) s + n;
    src = (unsigned char *) ct + n;
    while (n-- != 0) {
      *--dst = *--src;
    }
  }
  return s;
}
