/*
 * fpu.c -- floating-point unit
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "fpu.h"


#define FP_SGN(x)	((x) & 0x80000000)
#define FP_EXP(x)	(((x) << 1) >> 24)
#define FP_FRC(x)	((x) & 0x7FFFFF)


typedef union {
  Word w;
  float f;
} FP_Union;


Word fpAdd(Word x, Word y) {
  FP_Union X, Y, Z;

  X.w = x;
  Y.w = y;
  Z.f = X.f + Y.f;
  return Z.w;
}


Word fpSub(Word x, Word y) {
  FP_Union X, Y, Z;

  X.w = x;
  Y.w = y;
  Z.f = X.f - Y.f;
  return Z.w;
}


Word fpMul(Word x, Word y) {
  FP_Union X, Y, Z;

  X.w = x;
  Y.w = y;
  Z.f = X.f * Y.f;
  return Z.w;
}


Word fpDiv(Word x, Word y) {
  FP_Union X, Y, Z;

  X.w = x;
  Y.w = y;
  Z.f = X.f / Y.f;
  return Z.w;
}


int fpCmp(Word x, Word y) {
  FP_Union X, Y;

  if ((FP_EXP(x) == 0xFF && FP_FRC(x) != 0) ||
      (FP_EXP(y) == 0xFF && FP_FRC(y) != 0)) {
    return FP_CMP_UO;
  }
  X.w = x;
  Y.w = y;
  if (X.f < Y.f) {
    return FP_CMP_LT;
  }
  if (X.f > Y.f) {
    return FP_CMP_GT;
  }
  return FP_CMP_EQ;
}
