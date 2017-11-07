/*
 * utils.c -- utility functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}


void warning(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "Warning: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}


/**************************************************************/


void *memAlloc(unsigned int size) {
  void *p;

  p = malloc(size);
  if (p == NULL) {
    error("out of memory");
  }
  return p;
}


void memFree(void *p) {
  if (p == NULL) {
    error("memFree() got NULL pointer");
  }
  free(p);
}


/**************************************************************/


unsigned int read4FromEco(unsigned char *p) {
  return (unsigned int) p[0] << 24 |
         (unsigned int) p[1] << 16 |
         (unsigned int) p[2] <<  8 |
         (unsigned int) p[3] <<  0;
}


void write4ToEco(unsigned char *p, unsigned int data) {
  p[0] = data >> 24;
  p[1] = data >> 16;
  p[2] = data >>  8;
  p[3] = data >>  0;
}


void conv4FromEcoToNative(unsigned char *p) {
  unsigned int data;

  data = read4FromEco(p);
  * (unsigned int *) p = data;
}


void conv4FromNativeToEco(unsigned char *p) {
  unsigned int data;

  data = * (unsigned int *) p;
  write4ToEco(p, data);
}
