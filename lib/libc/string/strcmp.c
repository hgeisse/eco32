/*
 * strcmp.c -- string compare
 */


#include "string.h"


int strcmp(const char *cs, const char *ct) {
  while (*cs != '\0' && *cs == *ct) {
    cs++;
    ct++;
  }
  return (int) (unsigned char) *cs - (int) (unsigned char) *ct;
}
