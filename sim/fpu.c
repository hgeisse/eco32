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
