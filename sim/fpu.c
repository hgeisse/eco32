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


static Word fpFlags = 0;


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


Word fpCnvF2I(Word x) {
  FP_Union X;
  Word z;

  X.w = x;
  z = (Word) (int) X.f;
  return z;
}


Word fpCnvI2F(Word x) {
  FP_Union Z;

  Z.f = (float) (int) x;
  return Z.w;
}


int fpCmp(Word x, Word y, Bool invalidIfUnordered) {
  int xIsNaN;
  int yIsNaN;

  if ((x & 0x7FFFFFFF) == 0 && (y & 0x7FFFFFFF) == 0) {
    return FP_CMP_EQ;
  }
  xIsNaN = FP_EXP(x) == 0xFF && FP_FRC(x) != 0;
  yIsNaN = FP_EXP(y) == 0xFF && FP_FRC(y) != 0;
  if (xIsNaN || yIsNaN) {
    if ((xIsNaN && (FP_FRC(x) & 0x00400000) == 0) ||
        (yIsNaN && (FP_FRC(y) & 0x00400000) == 0) ||
        invalidIfUnordered) {
      fpFlags |= FP_V_FLAG;
    }
    return FP_CMP_UO;
  }
  if (FP_SGN(x) != (FP_SGN(y))) {
    return FP_SGN(x) ? FP_CMP_LT : FP_CMP_GT;
  }
  if (FP_SGN(x)) {
    if (x < y) {
      return FP_CMP_GT;
    }
    if (x > y) {
      return FP_CMP_LT;
    }
    return FP_CMP_EQ;
  } else {
    if (x < y) {
      return FP_CMP_LT;
    }
    if (x > y) {
      return FP_CMP_GT;
    }
    return FP_CMP_EQ;
  }
}
