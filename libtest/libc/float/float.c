/*
 * float.c -- show characteristic of floating-point types
 */


#include <stdio.h>
//#include <float.h>
#include "../../../lib/include/float.h"


int main(void) {
  printf("FLT_RADIX      : %d\n", FLT_RADIX);
  printf("----------------------------------------\n");
  printf("FLT_MANT_DIG   : %d\n", FLT_MANT_DIG);
  printf("FLT_DIG        : %d\n", FLT_DIG);
  printf("FLT_MIN_EXP    : %d\n", FLT_MIN_EXP);
  printf("FLT_MIN_10_EXP : %d\n", FLT_MIN_10_EXP);
  printf("FLT_MAX_EXP    : %d\n", FLT_MAX_EXP);
  printf("FLT_MAX_10_EXP : %d\n", FLT_MAX_10_EXP);
  printf("FLT_MAX        : %.12e\n", FLT_MAX);
  printf("FLT_EPSILON    : %.12e\n", FLT_EPSILON);
  printf("FLT_MIN        : %.12e\n", FLT_MIN);
  printf("----------------------------------------\n");
  printf("DBL_MANT_DIG   : %d\n", DBL_MANT_DIG);
  printf("DBL_DIG        : %d\n", DBL_DIG);
  printf("DBL_MIN_EXP    : %d\n", DBL_MIN_EXP);
  printf("DBL_MIN_10_EXP : %d\n", DBL_MIN_10_EXP);
  printf("DBL_MAX_EXP    : %d\n", DBL_MAX_EXP);
  printf("DBL_MAX_10_EXP : %d\n", DBL_MAX_10_EXP);
  printf("DBL_MAX        : %.12e\n", DBL_MAX);
  printf("DBL_EPSILON    : %.12e\n", DBL_EPSILON);
  printf("DBL_MIN        : %.12e\n", DBL_MIN);
  printf("----------------------------------------\n");
  printf("FLT_ROUNDS     : %d\n", FLT_ROUNDS);
  return 0;
}
