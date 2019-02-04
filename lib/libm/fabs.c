/*
 * fabs.c -- absolute value
 */


#include "math.h"
#include "sys/fp.h"


double fabs(double x) {
  _FP_Union X;

  X.f = x;
  X.w &= ~0x80000000;
  return X.f;
}
