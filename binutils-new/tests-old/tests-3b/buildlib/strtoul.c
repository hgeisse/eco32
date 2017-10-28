#include "romlib.h"


/*
 * Determine if a character is 'white space'.
 */
static Bool isspace(char c) {
  Bool res;

  switch (c) {
    case ' ':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
    case '\v':
      res = true;
      break;
    default:
      res = false;
      break;
  }
  return res;
}


/*
 * Check for valid digit, and convert to value.
 */
static Bool checkDigit(char c, int base, int *value) {
  if (c >= '0' && c <= '9') {
    *value = c - '0';
  } else
  if (c >= 'A' && c <= 'Z') {
    *value = c - 'A' + 10;
  } else
  if (c >= 'a' && c <= 'z') {
    *value = c - 'a' + 10;
  } else {
    return false;
  }
  return *value < base;
}


/*
 * Convert initial part of string to unsigned long integer.
 */
unsigned long strtoul(const char *s, char **endp, int base) {
  unsigned long res;
  int sign;
  int digit;

  res = 0;
  while (isspace(*s)) {
    s++;
  }
  if (*s == '+') {
    sign = 1;
    s++;
  } else
  if (*s == '-') {
    sign = -1;
    s++;
  } else {
    sign = 1;
  }
  if (base == 0 || base == 16) {
    if (*s == '0' &&
        (*(s + 1) == 'x' || *(s + 1) == 'X')) {
      /* base is 16 */
      s += 2;
      base = 16;
    } else {
      /* base is 0 or 16, but number does not start with "0x" */
      if (base == 0) {
        if (*s == '0') {
          s++;
          base = 8;
        } else {
          base = 10;
        }
      } else {
        /* take base as is */
      }
    }
  } else {
    /* take base as is */
  }
  while (checkDigit(*s, base, &digit)) {
    res *= base;
    res += digit;
    s++;
  }
  if (endp != NULL) {
    *endp = (char *) s;
  }
  return sign * res;
}
