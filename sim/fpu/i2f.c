/*
 * i2f.c -- convert integer to float
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
#include <math.h>

#include "../common.h"
#include "../console.h"
#include "../error.h"
#include "../fpu.h"
#include "fpubits.h"


Word fpuCnvI2F(Word x) {
  int aux;
  _FP_Union Z;
  int flags;

  aux = (int) x;
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
  Z.f = (float) aux;
  flags = fetestexcept(FE_ALL_EXCEPT);
  fpc |=
    ((flags & FE_INVALID)   ? FPU_FLAG_V : 0) |
    ((flags & FE_DIVBYZERO) ? FPU_FLAG_I : 0) |
    ((flags & FE_OVERFLOW)  ? FPU_FLAG_O : 0) |
    ((flags & FE_UNDERFLOW) ? FPU_FLAG_U : 0) |
    ((flags & FE_INEXACT)   ? FPU_FLAG_X : 0);
  return Z.w;
}