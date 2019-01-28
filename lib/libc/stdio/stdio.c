/*
 * !!!!! preliminary stdio.c !!!!!
 */


#include "stdio.h"


#include "prelimio.c"


FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;


int fflush(FILE *stream) {
  return 0;
}


void perror(const char *s) {
}
