/*
 * cmp.c -- comparison
 */


/*
 * !!!!!
 * This file must be replaced by an appropriate ECO32 implementation!
 * !!!!!
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fenv.h>

#include "../common.h"
#include "../console.h"
#include "../error.h"
#include "fpu.h"
#include "fpubits.h"


void fpuCmp(Word x, Word y, int pred) {
  _FP_Union X;
  _FP_Union Y;
  Bool cond;
  int flags;

  X.w = x;
  Y.w = y;
  switch (fpc & FPU_RND_MASK) {
    case FPU_RND_NEAR:
      fesetround(FE_TONEAREST);
      break;
    case FPU_RND_ZERO:
      fesetround(FE_TOWARDZERO);
      break;
    case FPU_RND_DOWN:
      fesetround(FE_DOWNWARD);
      break;
    case FPU_RND_UP:
      fesetround(FE_UPWARD);
      break;
  }
  feclearexcept(FE_ALL_EXCEPT);
  cond = false;
  switch (pred) {
    case FPU_PRED_EQ:
      if (X.f == Y.f) {
        cond = true;
      }
      break;
    case FPU_PRED_NE:
      if (X.f != Y.f) {
        cond = true;
      }
      break;
    case FPU_PRED_LE:
      if (X.f <= Y.f) {
        cond = true;
      }
      break;
    case FPU_PRED_LT:
      if (X.f < Y.f) {
        cond = true;
      }
      break;
    case FPU_PRED_ULE:
      if (X.f <= Y.f || X.f != X.f || Y.f != Y.f) {
        cond = true;
      }
      break;
    case FPU_PRED_ULT:
      if (X.f < Y.f || X.f != X.f || Y.f != Y.f) {
        cond = true;
      }
      break;
  }
  flags = fetestexcept(FE_ALL_EXCEPT);
  fpc |=
    ((flags & FE_INVALID)   ? FPU_FLAG_V : 0) |
    ((flags & FE_DIVBYZERO) ? FPU_FLAG_I : 0) |
    ((flags & FE_OVERFLOW)  ? FPU_FLAG_O : 0) |
    ((flags & FE_UNDERFLOW) ? FPU_FLAG_U : 0) |
    ((flags & FE_INEXACT)   ? FPU_FLAG_X : 0);
  if (cond) {
    fpc |= FPU_COND;
  } else {
    fpc &= ~FPU_COND;
  }
}
