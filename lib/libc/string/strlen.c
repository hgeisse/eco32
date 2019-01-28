/*
 * strlen.c -- string length
 */


#include "string.h"


size_t strlen(const char *cs) {
  size_t len;

  len = 0;
  while (*cs++ != '\0') {
    len++;
  }
  return len;
}
