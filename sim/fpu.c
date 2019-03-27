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


/**************************************************************/


#define FP_SGN(x)	((x) & 0x80000000)
#define FP_EXP(x)	(((x) << 1) >> 24)
#define FP_FRC(x)	((x) & 0x7FFFFF)
#define FP_FLT(s,e,f)	((s) | ((e) << 23) | (f))


typedef union {
  Word w;
  float f;
} FP_Union;


static Word fpuFlags;


/**************************************************************/


static Word nlz32(Word x) {
  Word z;

  z = 0;
  if (x == 0) {
    return 32;
  }
  if (x <= 0x0000FFFF) {
    z += 16;
    x <<= 16;
  }
  if (x <= 0x00FFFFFF) {
    z += 8;
    x <<= 8;
  }
  if (x <= 0x0FFFFFFF) {
    z += 4;
    x <<= 4;
  }
  if (x <= 0x3FFFFFFF) {
    z += 2;
    x <<= 2;
  }
  if (x <= 0x7FFFFFFF) {
    z += 1;
  }
  return z;
}


/**************************************************************/


Word fpuAdd(Word x, Word y) {
  FP_Union X, Y, Z;

  X.w = x;
  Y.w = y;
  Z.f = X.f + Y.f;
  return Z.w;
}


Word fpuSub(Word x, Word y) {
  FP_Union X, Y, Z;

  X.w = x;
  Y.w = y;
  Z.f = X.f - Y.f;
  return Z.w;
}


Word fpuMul(Word x, Word y) {
  FP_Union X, Y, Z;

  X.w = x;
  Y.w = y;
  Z.f = X.f * Y.f;
  return Z.w;
}


Word fpuDiv(Word x, Word y) {
  FP_Union X, Y, Z;

  X.w = x;
  Y.w = y;
  Z.f = X.f / Y.f;
  return Z.w;
}


/**************************************************************/


Word fpuCnvF2I(Word x) {
  int expX;
  Word frcX;
  Word z;

  expX = (int) FP_EXP(x) - 127;
  if (expX < 0) {
    return 0x00000000;
  }
  frcX = FP_FRC(x) | 0x00800000;
  if (expX <= 23) {
    z = frcX >> (23 - expX);
  } else
  if (expX <= 30) {
    z = frcX << (expX - 23);
  } else {
    if (x != 0xCF000000) {
      fpuFlags |= FPU_V_FLAG;
    }
    return 0x80000000;
  }
  return FP_SGN(x) ? -z : z;
}


Word fpuCnvI2F(Word x) {
  Word signX;
  int n;
  Word frcZ;
  int r, s;
  Word z;

  if (x == 0x00000000) {
    return 0x00000000;
  }
  signX = x & 0x80000000;
  if (signX) {
    x = -x;
  }
  n = nlz32(x);
  if (n >= 8) {
    frcZ = x << (n - 8);
    r = 0;
    s = 0;
  } else {
    frcZ = x >> (8 - n);
    r = (x & (0x00000001 << (7 - n))) != 0;
    s = (x & ((0x00000001 << (7 - n)) - 1)) != 0;
  }
  if (r | s) {
    fpuFlags |= FPU_X_FLAG;
  }
  z = FP_FLT(signX, 158 - n, frcZ & 0x007FFFFF) + (r & (frcZ | s));
  return z;
}


/**************************************************************/


int fpuCmp(Word x, Word y, Bool invalidIfUnordered) {
  int xIsNaN;
  int yIsNaN;

  if ((x & 0x7FFFFFFF) == 0 && (y & 0x7FFFFFFF) == 0) {
    return FPU_CMP_EQ;
  }
  xIsNaN = FP_EXP(x) == 0xFF && FP_FRC(x) != 0;
  yIsNaN = FP_EXP(y) == 0xFF && FP_FRC(y) != 0;
  if (xIsNaN || yIsNaN) {
    if ((xIsNaN && (FP_FRC(x) & 0x00400000) == 0) ||
        (yIsNaN && (FP_FRC(y) & 0x00400000) == 0) ||
        invalidIfUnordered) {
      fpuFlags |= FPU_V_FLAG;
    }
    return FPU_CMP_UO;
  }
  if (FP_SGN(x) != (FP_SGN(y))) {
    return FP_SGN(x) ? FPU_CMP_LT : FPU_CMP_GT;
  }
  if (FP_SGN(x)) {
    if (x < y) {
      return FPU_CMP_GT;
    }
    if (x > y) {
      return FPU_CMP_LT;
    }
    return FPU_CMP_EQ;
  } else {
    if (x < y) {
      return FPU_CMP_LT;
    }
    if (x > y) {
      return FPU_CMP_GT;
    }
    return FPU_CMP_EQ;
  }
}


/**************************************************************/


void fpuReset(void) {
  cPrintf("Resetting FPU...\n");
  fpuFlags = 0;
}


void fpuInit(void) {
  fpuReset();
}


void fpuExit(void) {
}
