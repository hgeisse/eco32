/*
 * fabs.c -- absolute value
 */


#include "math.h"


typedef unsigned int Word;

typedef union {
  float f;
  Word w;
} FP_Word;


double fabs(double x) {
  FP_Word X;

  X.f = x;
  X.w &= ~0x80000000;
  return X.f;
}
