/*
 * memcmp.c -- compare memory contents
 */


#include "string.h"


int memcmp(const void *cs, const void *ct, size_t n) {
  unsigned char *dst;
  unsigned char *src;

  dst = (unsigned char *) cs;
  src = (unsigned char *) ct;
  while (n-- != 0) {
    if (*dst != *src) {
      return (int) *dst - (int) *src;
    }
    dst++;
    src++;
  }
  return 0;
}
