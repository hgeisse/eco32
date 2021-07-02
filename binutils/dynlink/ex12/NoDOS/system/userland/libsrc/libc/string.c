/*
 * string.c -- string library
 */


#include "string.h"


/**************************************************************/

/* character string functions */


int strlen(char *str) {
  int i;

  i = 0;
  while (*str++ != '\0') {
    i++;
  }
  return i;
}


int strcmp(char *s1, char *s2) {
  while (*s1 == *s2) {
    if (*s1 == '\0') {
      return 0;
    }
    s1++;
    s2++;
  }
  return *s1 - *s2;
}


void strcpy(char *dst, char *src) {
  while ((*dst++ = *src++) != '\0') ;
}


void strcat(char *dst, char *src) {
  while (*dst++ != '\0') ;
  dst--;
  while ((*dst++ = *src++) != '\0') ;
}


char *strchr(const char *cs, int c) {
  do {
    if (*cs == c) {
      return (char *) cs;
    }
  } while (*cs++ != '\0');
  return NULL;
}


char *strtok(char *s, char *ct) {
  static char *p;
  char *q;

  if (s != NULL) {
    p = s;
  } else {
    p++;
  }
  while (*p != '\0' && strchr(ct, *p) != NULL) {
    p++;
  }
  if (*p == '\0') {
    return NULL;
  }
  q = p++;
  while (*p != '\0' && strchr(ct, *p) == NULL) {
    p++;
  }
  if (*p != '\0') {
    *p = '\0';
  } else {
    p--;
  }
  return q;
}


/**************************************************************/

/* byte string functions */


void memcpy(unsigned char *dst, unsigned char *src, unsigned int cnt) {
  while (cnt--) {
    *dst++ = *src++;
  }
}


void memset(unsigned char *dst, unsigned char c, unsigned int cnt) {
  while (cnt--) {
    *dst++ = c;
  }
}
