/*
 * strcpy.c -- string copy
 */


#include "string.h"


char *strcpy(char *s, const char *ct) {
  char *dst;

  dst = s;
  while ((*dst++ = *ct++) != '\0') ;
  return s;
}
