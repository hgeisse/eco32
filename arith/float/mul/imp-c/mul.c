/*
 * mul.c -- implementation and test of multiplication
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../include/fp.h"


typedef enum { false = 0, true = 1 } Bool;


/**************************************************************/

/*
 * display functions
 */


void dump(_FP_Word w) {
  printf("0x%08X = [%c%02X.%06X]",
         w, _FP_SGN(w) ? '-' : '+', _FP_EXP(w), _FP_FRC(w));
}


void showClass(_FP_Word w) {
  _FP_Word e;
  _FP_Word f;

  e = _FP_EXP(w);
  f = _FP_FRC(w);
  printf("<%s", _FP_SGN(w) ? "-" : "+");
  switch (e) {
    case 0:
      if (f == 0) {
        printf("Zero");
      } else {
        printf("Subn");
      }
      break;
    case 255:
      if (f == 0) {
        printf("Infn");
      } else {
        if (f & 0x00400000) {
          printf("qNaN");
        } else {
          printf("sNaN");
        }
      }
      break;
    default:
      printf("Nrml");
      break;
  }
  printf(">");
}


void showValue(_FP_Word w) {
  _FP_Union X;

  X.w = w;
  printf("%e", X.f);
}


void show(char *name, _FP_Word w) {
  printf("%s = ", name);
  dump(w);
  printf(" = ");
  showClass(w);
  printf(" = ");
  showValue(w);
}


void showFlags(_FP_Word flags) {
  printf("%c", (flags & _FP_V_FLAG) ? 'v' : '.');
  printf("%c", (flags & _FP_I_FLAG) ? 'i' : '.');
  printf("%c", (flags & _FP_O_FLAG) ? 'o' : '.');
  printf("%c", (flags & _FP_U_FLAG) ? 'u' : '.');
  printf("%c", (flags & _FP_X_FLAG) ? 'x' : '.');
}


/**************************************************************/

/*
 * 24x24-bit multiplier, implementation
 */


void mul24(_FP_Word x, _FP_Word y, _FP_Word *zhp, _FP_Word *zlp) {
  unsigned long z;

  x &= 0x00FFFFFF;
  y &= 0x00FFFFFF;
  z = (unsigned long) x * (unsigned long) y;
  *zhp = z >> 32;
  *zlp = z & 0xFFFFFFFF;
}


/**************************************************************/

/*
 * 24x24-bit multiplier, internal reference
 */


void mul24_ref(_FP_Word x, _FP_Word y, _FP_Word *zhp, _FP_Word *zlp) {
  _FP_Word upper, lower;
  Bool carry;
  int i;

  upper = 0;
  lower = x;
  for (i = 0; i < 24; i++) {
    if (lower & 1) {
      upper = upper + y;
      carry = upper < y;
    } else {
      upper = upper + 0;
      carry = false;
    }
    lower >>= 1;
    if (upper & 1) {
      lower |= 0x80000000;
    }
    upper >>= 1;
    if (carry) {
      upper |= 0x80000000;
    }
  }
  lower = lower >> 8;
  lower = (upper << 24) | lower;
  upper = upper >> 8;
  *zhp = upper;
  *zlp = lower;
}


/**************************************************************/

/*
 * 24x24-bit multiplier, check
 */


#define SKIP_MASK_24	0x00007FF8


void check_mul24(void) {
  _FP_Word x, y;
  _FP_Word zh, zl;
  _FP_Word rh, rl;
  long count, errors;

  count = 0;
  errors = 0;
  for (x = 0; x < (1 << 24); x++) {
    if ((x & 0x0FFFFF) == 0) {
      printf("reached test loop 0x%08X\n", x);
    }
    if ((x & SKIP_MASK_24) != 0x00000000 &&
        (x & SKIP_MASK_24) != SKIP_MASK_24) {
      x |= SKIP_MASK_24;
    }
    for (y = 0; y < (1 << 24); y++) {
      if ((y & SKIP_MASK_24) != 0x00000000 &&
          (y & SKIP_MASK_24) != SKIP_MASK_24) {
        y |= SKIP_MASK_24;
      }
      mul24(x, y, &zh, &zl);
      mul24_ref(x, y, &rh, &rl);
      count++;
      if (zh != rh || zl != rl) {
        errors++;
      }
    }
  }
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
}


/**************************************************************/

/*
 * 24-bit leading-zero counter, implementation
 */


typedef unsigned char Byte;


Byte encode(Byte two_bits) {
  Byte r;

  switch (two_bits) {
    case 0:
      r = 2;
      break;
    case 1:
      r = 1;
      break;
    case 2:
      r = 0;
      break;
    case 3:
      r = 0;
      break;
  }
  return r;
}


Byte combine1(Byte right, Byte left) {
  Byte r;

  if ((left & 2) != 0 && (right & 2) != 0) {
    r = 4;
  } else
  if ((left & 2) == 0) {
    r = left;
  } else {
    r = right | 2;
  }
  return r;
}


Byte combine2(Byte right, Byte left) {
  Byte r;

  if ((left & 4) != 0 && (right & 4) != 0) {
    r = 8;
  } else
  if ((left & 4) == 0) {
    r = left;
  } else {
    r = right | 4;
  }
  return r;
}


Byte combine3(Byte right, Byte left) {
  Byte r;

  if ((left & 8) != 0 && (right & 8) != 0) {
    r = 16;
  } else
  if ((left & 8) == 0) {
    r = left;
  } else {
    r = right | 8;
  }
  return r;
}


int lzc24(_FP_Word m) {
  Byte lz_0_0, lz_0_1, lz_0_2, lz_0_3, lz_0_4, lz_0_5;
  Byte lz_0_6, lz_0_7, lz_0_8, lz_0_9, lz_0_A, lz_0_B;
  Byte lz_1_0, lz_1_1, lz_1_2, lz_1_3, lz_1_4, lz_1_5;
  Byte lz_2_0, lz_2_1, lz_2_2;
  Byte lz_3_0, lz_3_1;
  Byte lz_4_0;

  /* stage 0: encode 2 bits */
  lz_0_0 = encode((m >>  0) & 0x03);
  lz_0_1 = encode((m >>  2) & 0x03);
  lz_0_2 = encode((m >>  4) & 0x03);
  lz_0_3 = encode((m >>  6) & 0x03);
  lz_0_4 = encode((m >>  8) & 0x03);
  lz_0_5 = encode((m >> 10) & 0x03);
  lz_0_6 = encode((m >> 12) & 0x03);
  lz_0_7 = encode((m >> 14) & 0x03);
  lz_0_8 = encode((m >> 16) & 0x03);
  lz_0_9 = encode((m >> 18) & 0x03);
  lz_0_A = encode((m >> 20) & 0x03);
  lz_0_B = encode((m >> 22) & 0x03);
  /* stage 1: symmetrical combine 2+2 bits */
  lz_1_0 = combine1(lz_0_0, lz_0_1);
  lz_1_1 = combine1(lz_0_2, lz_0_3);
  lz_1_2 = combine1(lz_0_4, lz_0_5);
  lz_1_3 = combine1(lz_0_6, lz_0_7);
  lz_1_4 = combine1(lz_0_8, lz_0_9);
  lz_1_5 = combine1(lz_0_A, lz_0_B);
  /* stage 2: symmetrical combine 4+4 bits */
  lz_2_0 = combine2(lz_1_0, lz_1_1);
  lz_2_1 = combine2(lz_1_2, lz_1_3);
  lz_2_2 = combine2(lz_1_4, lz_1_5);
  /* stage 3: symmetrical combine 8+8 bits, leave top 8 bits alone */
  lz_3_0 = combine3(lz_2_0, lz_2_1);
  lz_3_1 = lz_2_2;
  /* stage4: asymmetrical combine 8+16 bits */
  if (lz_3_1 & 8) {
    lz_4_0 = 8 + lz_3_0;
  } else {
    lz_4_0 = lz_3_1;
  }
  return lz_4_0;
}


/**************************************************************/

/*
 * 24-bit leading-zero counter, internal reference
 */


int lzc24_ref(_FP_Word m) {
  int i;

  for (i = 23; i >= 0; i--) {
    if (m & (1 << i)) {
      return 23 - i;
    }
  }
  return 24;
}


/**************************************************************/

/*
 * 24-bit leading-zero counter, check
 */


void check_lzc24(void) {
  _FP_Word m;
  int lz, lz_ref;
  long count, errors;

  count = 0;
  errors = 0;
  for (m = 0; m < (1 << 24); m++) {
    lz = lzc24(m);
    lz_ref = lzc24_ref(m);
    count++;
    if (lz != lz_ref) {
      errors++;
    }
  }
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
}


/**************************************************************/

/*
 * floating-point multiplier, implementation
 */


#define DEVELOPING	0

#if DEVELOPING
_FP_Word fake_fpMul(_FP_Word x, _FP_Word y) {
  extern _FP_Word Flags;
  extern _FP_Word Flags_ref;
  extern _FP_Word fpMul_ref(_FP_Word x, _FP_Word y);
  _FP_Word Flags_save;
  _FP_Word z;

  Flags_save = Flags_ref;
  Flags_ref = 0;
  z = fpMul_ref(x, y);
  Flags = Flags_ref;
  Flags_ref = Flags_save;
  return z;
}
#endif


#define genZERO		((_FP_Word) 0x00000000)
#define genNAN		((_FP_Word) 0x7FC00000)
#define genINF		((_FP_Word) 0x7F800000)

#define QUIET_BIT	((_FP_Word) 0x00400000)
#define IS_QUIET(f)	((f & QUIET_BIT) != 0)

#define IS_ZERO(e,f)	((e) ==   0 && (f) ==   0)
#define IS_SUB(e,f)	((e) ==   0 && (f) !=   0)
#define IS_INF(e,f)	((e) == 255 && (f) ==   0)
#define IS_NAN(e,f)	((e) == 255 && (f) !=   0)
#define IS_NORM(e)	((e) !=   0 && (e) != 255)
#define IS_NUM(e,f)	(IS_NORM(e) || IS_SUB(e, f))


Bool debug = false;

_FP_Word Flags = 0;


_FP_Word fpMul(_FP_Word x, _FP_Word y) {
  _FP_Word sx, ex, fx;
  _FP_Word sy, ey, fy;
  _FP_Word sz, ez, fz;
  Bool nx;
  _FP_Word mx;
  int lx;
  _FP_Word mxnorm;
  int exnorm;
  Bool ny;
  _FP_Word my;
  int ly;
  _FP_Word mynorm;
  int eynorm;
  _FP_Word fzh, fzl;
  _FP_Word fzx1, fzx2;
  _FP_Word z;
  Bool round, sticky, odd, incr;
  int shift;

  if (debug) {
    printf("------------------------------------------------\n");
    show("x", x);
    printf("\n");
    show("y", y);
    printf("\n");
  }
  sx = _FP_SGN(x);
  ex = _FP_EXP(x);
  fx = _FP_FRC(x);
  if (debug) {
    printf("x: sx = %d, ex = %3d, fx = 0x%06X\n", sx, ex, fx);
  }
  sy = _FP_SGN(y);
  ey = _FP_EXP(y);
  fy = _FP_FRC(y);
  if (debug) {
    printf("y: sy = %d, ey = %3d, fy = 0x%06X\n", sy, ey, fy);
  }
  sz = sx ^ sy;
  if (IS_ZERO(ex, fx) && IS_ZERO(ey, fy)) {
    ez = _FP_EXP(genZERO);
    fz = _FP_FRC(genZERO);
    z = _FP_FLT(sz, ez, fz);
  } else
  if (IS_ZERO(ex, fx) && IS_INF(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = _FP_FRC(genNAN);
    z = _FP_FLT(sz, ez, fz);
    Flags |= _FP_V_FLAG;
  } else
  if (IS_ZERO(ex, fx) && IS_NAN(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = fy | QUIET_BIT;
    z = _FP_FLT(sz, ez, fz);
    if (!IS_QUIET(fy)) {
      Flags |= _FP_V_FLAG;
    }
  } else
  if (IS_ZERO(ex, fx) && IS_NUM(ey, fy)) {
    ez = _FP_EXP(genZERO);
    fz = _FP_FRC(genZERO);
    z = _FP_FLT(sz, ez, fz);
  } else
  if (IS_INF(ex, fx) && IS_ZERO(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = _FP_FRC(genNAN);
    z = _FP_FLT(sz, ez, fz);
    Flags |= _FP_V_FLAG;
  } else
  if (IS_INF(ex, fx) && IS_INF(ey, fy)) {
    ez = _FP_EXP(genINF);
    fz = _FP_FRC(genINF);
    z = _FP_FLT(sz, ez, fz);
  } else
  if (IS_INF(ex, fx) && IS_NAN(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = fy | QUIET_BIT;
    z = _FP_FLT(sz, ez, fz);
    if (!IS_QUIET(fy)) {
      Flags |= _FP_V_FLAG;
    }
  } else
  if (IS_INF(ex, fx) && IS_NUM(ey, fy)) {
    ez = _FP_EXP(genINF);
    fz = _FP_FRC(genINF);
    z = _FP_FLT(sz, ez, fz);
  } else
  if (IS_NAN(ex, fx) && IS_ZERO(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = fx | QUIET_BIT;
    z = _FP_FLT(sz, ez, fz);
    if (!IS_QUIET(fx)) {
      Flags |= _FP_V_FLAG;
    }
  } else
  if (IS_NAN(ex, fx) && IS_INF(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = fx | QUIET_BIT;
    z = _FP_FLT(sz, ez, fz);
    if (!IS_QUIET(fx)) {
      Flags |= _FP_V_FLAG;
    }
  } else
  if (IS_NAN(ex, fx) && IS_NAN(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = fx | QUIET_BIT;
    z = _FP_FLT(sz, ez, fz);
    if (!IS_QUIET(fx) || !IS_QUIET(fy)) {
      Flags |= _FP_V_FLAG;
    }
  } else
  if (IS_NAN(ex, fx) && IS_NUM(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = fx | QUIET_BIT;
    z = _FP_FLT(sz, ez, fz);
    if (!IS_QUIET(fx)) {
      Flags |= _FP_V_FLAG;
    }
  } else
  if (IS_NUM(ex, fx) && IS_ZERO(ey, fy)) {
    ez = _FP_EXP(genZERO);
    fz = _FP_FRC(genZERO);
    z = _FP_FLT(sz, ez, fz);
  } else
  if (IS_NUM(ex, fx) && IS_INF(ey, fy)) {
    ez = _FP_EXP(genINF);
    fz = _FP_FRC(genINF);
    z = _FP_FLT(sz, ez, fz);
  } else
  if (IS_NUM(ex, fx) && IS_NAN(ey, fy)) {
    ez = _FP_EXP(genNAN);
    fz = fy | QUIET_BIT;
    z = _FP_FLT(sz, ez, fz);
    if (!IS_QUIET(fy)) {
      Flags |= _FP_V_FLAG;
    }
  } else {
    if (!IS_NUM(ex, fx) || !IS_NUM(ey, fy)) {
      fprintf(stderr, "FATAL INTERNAL ERROR 1\n");
      exit(1);
    }
    /* stage 1: normalize inputs */
    nx = IS_NORM(ex);
    mx = (nx << 23) | fx;
    lx = lzc24(mx);
    mxnorm = mx << lx;
    exnorm = ex - 127 + (1 - nx) - lx;
    if (debug) {
      printf("nx = %d, mx = 0x%08X, lx = %d, mxnorm = 0x%08X, exnorm = %d\n",
             nx, mx, lx, mxnorm, exnorm);
    }
    ny = IS_NORM(ey);
    my = (ny << 23) | fy;
    ly = lzc24(my);
    mynorm = my << ly;
    eynorm = ey - 127 + (1 - ny) - ly;
    if (debug) {
      printf("ny = %d, my = 0x%08X, ly = %d, mynorm = 0x%08X, eynorm = %d\n",
             ny, my, ly, mynorm, eynorm);
    }
    /* stage 2: multiply significands, compute exponent */
    mul24(mxnorm, mynorm, &fzh, &fzl);
    ez = exnorm + eynorm + 127;
    if (debug) {
      printf("fzh = 0x%08X, fzl = 0x%08X, ez = %d\n", fzh, fzl, ez);
    }
    if ((fzh & (1 << 15)) != 0) {
      if (debug) {
        printf("fz is in [2, 4)\n");
      }
      ez++;
      fz = (fzh << 8) | (fzl >> (32 - 8));
      fzx1 = fzl << 8;
    } else {
      if (debug) {
        printf("fz is in [1, 2)\n");
      }
      fz = (fzh << 9) | (fzl >> (32 - 9));
      fzx1 = fzl << 9;
    }
    if (debug) {
      printf("fz = 0x%08X, fzx1 = 0x%08X, ez = %d\n", fz, fzx1, ez);
    }
    /* stage 3: handle overflow, denormal and normal results, rounding */
    if ((int) ez >= 255) {
      /* overflow */
      ez = _FP_EXP(genINF);
      fz = _FP_FRC(genINF);
      z = _FP_FLT(sz, ez, fz);
      Flags |= _FP_O_FLAG | _FP_X_FLAG;
    } else
    if ((int) ez <= 0) {
      /* denormal result */
      shift = -ez + 1;
      if (debug) {
        printf("denorm shift = %d\n", shift);
      }
      if (shift <= 24) {
        fzx2 = fzx1 << (32 - shift);
        fzx1 = (fz << (32 - shift)) | (fzx1 >> shift);
        fz >>= shift;
        if (debug) {
          printf("fz = 0x%08X, fzx1 = 0x%08X, fzx2 = 0x%08X\n",
                 fz, fzx1, fzx2);
        }
        round = ((fzx1 & (1 << 31)) != 0);
        sticky = (((fzx1 << 1) | fzx2) != 0);
        odd = ((fz & 1) != 0);
        incr = round && (sticky || odd);
        if (debug) {
          printf("round = %d, sticky = %d, odd = %d => incr = %d\n",
                 round, sticky, odd, incr);
        }
        z = _FP_FLT(sz, 0, fz);
        if (incr) {
          z++;
        }
        if (round || sticky) {
          Flags |= _FP_U_FLAG | _FP_X_FLAG;
        }
      } else {
        z = _FP_FLT(sz, 0, 0);
        Flags |= _FP_U_FLAG | _FP_X_FLAG;
      }
    } else {
      /* normal result */
      fz = fz & ~(1 << 23);
      round = ((fzx1 & (1 << 31)) != 0);
      sticky = ((fzx1 << 1) != 0);
      odd = ((fz & 1) != 0);
      incr = round && (sticky || odd);
      if (debug) {
        printf("round = %d, sticky = %d, odd = %d => incr = %d\n",
               round, sticky, odd, incr);
      }
      z = _FP_FLT(sz, ez, fz);
      if (incr) {
        z++;
      }
      if (round || sticky) {
        Flags |= _FP_X_FLAG;
      }
      if (_FP_EXP(z) == 255) {
        Flags |= _FP_O_FLAG;
      }
    }
  }
  if (debug) {
    show("z", z);
    printf("    ");
    showFlags(Flags);
    printf("\n");
    printf("------------------------------------------------\n");
  }
  return z;
}


/**************************************************************/

/*
 * floating-point multiplier, internal reference
 */


#include "softfloat.h"


_FP_Word Flags_ref = 0;


_FP_Word fpMul_ref(_FP_Word x, _FP_Word y) {
  float32_t X, Y, Z;

  X.v = x;
  Y.v = y;
  softfloat_detectTininess = softfloat_tininess_beforeRounding;
  softfloat_roundingMode = softfloat_round_near_even;
  softfloat_exceptionFlags = 0;
  Z = f32_mul(X, Y);
  Flags_ref |=
    ((softfloat_exceptionFlags & softfloat_flag_invalid)   ? _FP_V_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_infinite)  ? _FP_I_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_overflow)  ? _FP_O_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_underflow) ? _FP_U_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_inexact)   ? _FP_X_FLAG : 0);
  return Z.v;
}


/**************************************************************/

/*
 * floating-point comparison
 */


Bool compareEqual(_FP_Word x, _FP_Word y) {
  if (x == y) {
    return true;
  }
  if (_FP_EXP(x) == 255 && _FP_FRC(x) != 0 &&
      _FP_EXP(y) == 255 && _FP_FRC(y) != 0) {
    /* both are NaNs: compare quiet bits, but not signs nor payloads */
    return (x & 0x00400000) == (y & 0x00400000);
  }
  return false;
}


/**************************************************************/

/*
 * check the floating-point operation with simple values
 */


void check_simple(void) {
  int i, j;
  _FP_Word x, y;
  _FP_Word z, r;
  _FP_Union u;
  Bool error;

  /* [1.0, 20.0] */
  printf("------------------------------------------------\n");
  printf("interval = [1.0, 20.0]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    for (j = 1; j <= 20; j++) {
      u.f = (float) i;
      x = u.w;
      u.f = (float) j;
      y = u.w;
      Flags = 0;
      z = fpMul(x, y);
      Flags_ref = 0;
      r = fpMul_ref(x, y);
      show("r", r);
      printf("    ");
      showFlags(Flags_ref);
      printf("\n");
      if (!compareEqual(z, r)) {
        error = true;
        printf("value wrong");
      } else {
        printf("value ok");
      }
      printf(", ");
      if (Flags != Flags_ref) {
        error = true;
        printf("flags wrong");
      } else {
        printf("flags ok");
      }
      printf("\n");
      printf("------------------------------------------------\n");
      if (error) {
        printf("summary: stopped on first error\n");
        return;
      }
    }
  }
  /* [0.10, 2.00] */
  printf("------------------------------------------------\n");
  printf("interval = [0.10, 2.00]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    for (j = 1; j <= 20; j++) {
      u.f = (float) i / (float) 10.0;
      x = u.w;
      u.f = (float) j / (float) 10.0;
      y = u.w;
      Flags = 0;
      z = fpMul(x, y);
      Flags_ref = 0;
      r = fpMul_ref(x, y);
      show("r", r);
      printf("    ");
      showFlags(Flags_ref);
      printf("\n");
      if (!compareEqual(z, r)) {
        error = true;
        printf("value wrong");
      } else {
        printf("value ok");
      }
      printf(", ");
      if (Flags != Flags_ref) {
        error = true;
        printf("flags wrong");
      } else {
        printf("flags ok");
      }
      printf("\n");
      printf("------------------------------------------------\n");
      if (error) {
        printf("summary: stopped on first error\n");
        return;
      }
    }
  }
  printf("summary: no errors\n");
}


/**************************************************************/

/*
 * check the floating-point operation with selected values
 */


typedef struct {
  _FP_Word x;
  _FP_Word y;
} _FP_Pair;


_FP_Pair pairs[] = {
  /*
   * special values
   */
  /* { zero, inf } */
  { 0x00000000, 0x00000000 },
  { 0x7F800000, 0x7F800000 },
  { 0x00000000, 0x7F800000 },
  { 0x7F800000, 0x00000000 },
  /* { sNan, qNan } */
  { 0x7F800005, 0x7F800006 },
  { 0x7FC00005, 0x7FC00006 },
  { 0x7F800005, 0x7FC00006 },
  { 0x7FC00005, 0x7F800006 },
  /* { sNan, zero } */
  { 0x7F800005, 0x00000000 },
  { 0x00000000, 0x7F800006 },
  /* { sNan, inf } */
  { 0x7F800005, 0x7F800000 },
  { 0x7F800000, 0x7F800006 },
  /* { qNan, zero } */
  { 0x7FC00005, 0x00000000 },
  { 0x00000000, 0x7FC00006 },
  /* { qNan, inf } */
  { 0x7FC00005, 0x7F800000 },
  { 0x7F800000, 0x7FC00006 },
  /*
   * normal values, overflow
   */
  { 0x7F7FFFFF, 0x7F7FFFFF },
  { 0x60000000, 0x60000000 },
  { 0x5F800001, 0x5F7FFFFF },
  /*
   * normal values, normal result
   */
  { 0x3F800000, 0x3F800000 },
  { 0x3F800000, 0x40000000 },
  { 0x40000000, 0x40000000 },
  { 0x40000000, 0x40400000 },
  { 0x40400000, 0x40400000 },
  /*
   * normal values, subnormal result
   */
  { 0x1F800000, 0x1F800000 },
  /*
   * subnormal values
   */
  { 0x40000000, 0x00444444 },
  { 0x00444444, 0x40000000 },
  { 0x3F800000, 0x00444444 },
  { 0x00444444, 0x3F800000 },
  { 0x0006274F, 0xDE7FC1FF },
  /*
   * subnormal results
   */
  { 0x26A50F00, 0x1F86A50F },
  { 0x1F86A50F, 0x1F86A50F },
  { 0x1F6A50F0, 0x1F86A50F },
  { 0x1E86A50F, 0x1F86A50F },
  { 0x1E6A50F0, 0x1F86A50F },
  { 0x1D86A50F, 0x1F86A50F },
  { 0x1D6A50F0, 0x1F86A50F },
  { 0x1C86A50F, 0x1F86A50F },
  { 0x1C6A50F0, 0x1F86A50F },
  { 0x1B86A50F, 0x1F86A50F },
  { 0x1B6A50F0, 0x1F86A50F },
  { 0x1A86A50F, 0x1F86A50F },
  { 0x1A6A50F0, 0x1F86A50F },
  { 0x1986A50F, 0x1F86A50F },
  { 0x196A50F0, 0x1F86A50F },
  { 0x1886A50F, 0x1F86A50F },
  { 0x186A50F0, 0x1F86A50F },
  { 0x1786A50F, 0x1F86A50F },
  { 0x176A50F0, 0x1F86A50F },
  { 0x1686A50F, 0x1F86A50F },
  { 0x166A50F0, 0x1F86A50F },
  { 0x1586A50F, 0x1F86A50F },
  { 0x156A50F0, 0x1F86A50F },
  { 0x1486A50F, 0x1F86A50F },
  { 0x146A50F0, 0x1F86A50F },
  { 0x1386A50F, 0x1F86A50F },
  { 0x136A50F0, 0x1F86A50F },
  { 0x1286A50F, 0x1F86A50F },
  { 0x126A50F0, 0x1F86A50F },
  { 0x1186A50F, 0x1F86A50F },
  { 0x1106A50F, 0x1F86A50F },
  /*
   * other tests
   */
  { 0x3FABBF00, 0x3FABC000 },
  { 0x403FFFDF, 0x7EFFFFB0 },
  { 0x007FFC00, 0x3F800400 },
  { 0x00100000, 0x40FFFFFF },
  { 0x817FE003, 0x3E801000 },
  { 0x80080100, 0xC17FE003 },
};

int numPairs = sizeof(pairs)/sizeof(pairs[0]);


void check_selected(void) {
  _FP_Word x, y;
  _FP_Word z, r;
  int i;
  Bool error;

  error = false;
  for (i = 0; i < numPairs; i++) {
    x = pairs[i].x;
    y = pairs[i].y;
    Flags = 0;
    z = fpMul(x, y);
    Flags_ref = 0;
    r = fpMul_ref(x, y);
    show("r", r);
    printf("    ");
    showFlags(Flags_ref);
    printf("\n");
    if (!compareEqual(z, r)) {
      error = true;
      printf("value wrong");
    } else {
      printf("value ok");
    }
    printf(", ");
    if (Flags != Flags_ref) {
      error = true;
      printf("flags wrong");
    } else {
      printf("flags ok");
    }
    printf("\n");
    printf("------------------------------------------------\n");
    if (error) {
      printf("summary: stopped on first error\n");
      return;
    }
  }
  printf("summary: no errors\n");
}


/**************************************************************/

/*
 * check the floating-point operation in different intervals
 */


#define SKIP_MASK	0x001FFFFC


void check_intervals(void) {
  _FP_Word x, y;
  _FP_Word z, r;
  long count, errors;

  printf("------------------------------------------------\n");
  printf("Part 1: full small interval [0x3FABBBBB, 0x3FABCDEF]\n");
  count = 0;
  errors = 0;
  x = 0x3FABBBBB;
  do {
    y = 0x3FABBBBB;
    do {
      Flags = 0;
      z = fpMul(x, y);
      Flags_ref = 0;
      r = fpMul_ref(x, y);
      count++;
      if (!compareEqual(z, r) || Flags != Flags_ref) {
        if (errors == 0) {
          printf("first error: x = 0x%08X, y = 0x%08X\n", x, y);
          printf("             z = 0x%08X, r = 0x%08X\n", z, r);
        }
        errors++;
      }
      y++;
    } while (y <= 0x3FABCDEF);
    x++;
  } while (x <= 0x3FABCDEF);
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
  printf("------------------------------------------------\n");
  printf("Part 2: sparse bigger interval [0x3F000000, 0x40000000]\n");
  count = 0;
  errors = 0;
  x = 0x3F000000;
  do {
    y = 0x3F000000;
    do {
      Flags = 0;
      z = fpMul(x, y);
      Flags_ref = 0;
      r = fpMul_ref(x, y);
      count++;
      if (!compareEqual(z, r) || Flags != Flags_ref) {
        if (errors == 0) {
          printf("first error: x = 0x%08X, y = 0x%08X\n", x, y);
          printf("             z = 0x%08X, r = 0x%08X\n", z, r);
        }
        errors++;
      }
      y += 2999;
    } while (y <= 0x40000000);
    x += 3001;
  } while (x <= 0x40000000);
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
  printf("------------------------------------------------\n");
  printf("Part 3: full range, big gaps\n");
  count = 0;
  errors = 0;
  x = 0;
  do {
    if ((x & 0x0FFFFFFF) == 0) {
      printf("reached test loop 0x%08X\n", x);
    }
    if ((x & SKIP_MASK) != 0x00000000 &&
        (x & SKIP_MASK) != SKIP_MASK) {
      x |= SKIP_MASK;
    }
    y = 0;
    do {
      if ((y & SKIP_MASK) != 0x00000000 &&
          (y & SKIP_MASK) != SKIP_MASK) {
        y |= SKIP_MASK;
      }
      Flags = 0;
      z = fpMul(x, y);
      Flags_ref = 0;
      r = fpMul_ref(x, y);
      count++;
      if (!compareEqual(z, r) || Flags != Flags_ref) {
        if (errors == 0) {
          printf("first error: x = 0x%08X, y = 0x%08X\n", x, y);
          printf("             z = 0x%08X, r = 0x%08X\n", z, r);
        }
        errors++;
      }
      y++;
    } while (y != 0);
    x++;
  } while (x != 0);
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
  printf("------------------------------------------------\n");
}


/**************************************************************/

/*
 * make the floating-point operation available as a service
 * (used to check against an external reference implementation)
 */


#define LINE_SIZE	200


void server(void) {
  char line[LINE_SIZE];
  char *endptr;
  _FP_Word x, y, z;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    x = strtoul(endptr, &endptr, 16);
    y = strtoul(endptr, &endptr, 16);
    Flags = 0;
    z = fpMul(x, y);
    printf("%08X %08X %08X %02X\n", x, y, z, Flags);
  }
}


/**************************************************************/

/*
 * main program
 */


void usage(char *myself) {
  printf("usage: %s -mul24 | -lzc24 | -simple | "
         "-selected | -intervals | -server\n",
         myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc == 2 && strcmp(argv[1], "-mul24") == 0) {
    printf("Check 24x24-bit multiplier\n");
    check_mul24();
    return 0;
  }
  if (argc == 2 && strcmp(argv[1], "-lzc24") == 0) {
    printf("Check 24-bit leading-zero counter\n");
    check_lzc24();
    return 0;
  }
  if (argc == 2 && strcmp(argv[1], "-simple") == 0) {
    printf("Check simple test cases\n");
    debug = true;
    check_simple();
    return 0;
  }
  if (argc == 2 && strcmp(argv[1], "-selected") == 0) {
    printf("Check selected test cases\n");
    debug = true;
    check_selected();
    return 0;
  }
  if (argc == 2 && strcmp(argv[1], "-intervals") == 0) {
    printf("Check different intervals\n");
    check_intervals();
    return 0;
  }
  if (argc == 2 && strcmp(argv[1], "-server") == 0) {
    server();
    return 0;
  }
  usage(argv[0]);
  return 0;
}
