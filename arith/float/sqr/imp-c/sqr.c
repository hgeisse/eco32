/*
 * sqr.c -- implementation and test of square root
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../include/fp.h"


typedef enum { false = 0, true = 1 } Bool;


/**************************************************************/


#define ROUND_NEAR	0
#define ROUND_ZERO	1
#define ROUND_DOWN	2
#define ROUND_UP	3


int rounding;		/* one of near, zero, down, up */


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
 * 24-bit square root, implementation
 */


void sqr24(_FP_Word x, _FP_Word *rootp, _FP_Word *remp) {
  _FP_Word Q;
  _FP_Word R;
  _FP_Word S;
  _FP_Word T;
  int i;

  Q = 1 << 24;
  R = x - (1 << 24);
  S = 1 << 24;
  for (i = 1; i <= 24; i++) {
    S >>= 1;
    T = (R << 1) - ((Q << 1) + S);
    if ((int) T < 0) {
      R = R << 1;
    } else {
      Q = Q + S;
      R = T;
    }
  }
  *rootp = Q;
  *remp = R;
}


/**************************************************************/

/*
 * 24-bit square root, internal reference
 */


void sqr24_ref(_FP_Word x, _FP_Word *zqp, _FP_Word *zrp) {
  unsigned long lx;
  unsigned long lz0, lz1;
  _FP_Word quo, rem;

  lx = (unsigned long) x << 24;
  lz0 = (unsigned long) x << 11;
  lz1 = (lz0 + lx / lz0) >> 1;
  while (lz1 < lz0) {
    lz0 = lz1;
    lz1 = (lz0 + lx / lz0) >> 1;
  }
  quo = (_FP_Word) lz0;
  rem = (_FP_Word) (lx - lz0 * lz0);
  *zqp = quo;
  *zrp = rem;
}


/**************************************************************/

/*
 * 24-bit square root, check
 */


void check_sqr24(void) {
  _FP_Word x;
  _FP_Word zq, zr;
  _FP_Word rq, rr;
  long count, errors;

  printf("------------------------------------------------\n");
  printf("Part 1: Check reference internally\n");
  count = 0;
  for (x = (1 << 24); x < (1 << 26); x++) {
    if ((x & 0x3FFFFF) == 0) {
      printf("reached test loop 0x%08X\n", x);
    }
    sqr24_ref(x, &rq, &rr);
    count++;
    if ((unsigned long) rq * (unsigned long) rq +
        (unsigned long) rr != (unsigned long) x << 24) {
      printf("x = 0x%08X : rq = 0x%08X, rr = 0x%08X\n",
             x, rq, rr);
      printf("fatal error: rq * rq + rr != x << 24\n");
      exit(1);
    }
    if (((unsigned long) rq + 1) *
        ((unsigned long) rq + 1) <=
        (unsigned long) x << 24) {
      printf("x = 0x%08X : rq = 0x%08X, rr = 0x%08X\n",
             x, rq, rr);
      printf("fatal error: (rq + 1) * (rq + 1) <= x << 24\n");
      exit(1);
    }
  }
  printf("number of tests  = %ld\n", count);
  printf("no errors detected\n");
  printf("------------------------------------------------\n");
  printf("Part 2: Check implementation against reference\n");
  count = 0;
  errors = 0;
  for (x = (1 << 24); x < (1 << 26); x++) {
    if ((x & 0x3FFFFF) == 0) {
      printf("reached test loop 0x%08X\n", x);
    }
    sqr24(x, &zq, &zr);
    sqr24_ref(x, &rq, &rr);
    count++;
    if (zq != rq || zr != rr) {
      errors++;
    }
  }
  printf("number of tests  = %ld\n", count);
  printf("no errors detected\n");
}


/**************************************************************/

/*
 * 24-bit leading-zero counter, implementation
 */


typedef unsigned char Byte;


Byte encode2(Byte two_bits) {
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


Byte combine2(Byte left, Byte right) {
  Byte r;

  if ((left & 2) != 0 && (right & 2) != 0) {
    r = 4;
  } else
  if ((left & 2) != 0) {
    r = right | 2;
  } else {
    r = left;
  }
  return r;
}


Byte combine3(Byte left, Byte right) {
  Byte r;

  if ((left & 4) != 0 && (right & 4) != 0) {
    r = 8;
  } else
  if ((left & 4) != 0) {
    r = right | 4;
  } else {
    r = left;
  }
  return r;
}


Byte combine4(Byte left, Byte middle, Byte right) {
  Byte r;

  if ((left & 8) != 0 && (middle & 8) != 0 && (right & 8) != 0) {
    r = 24;
  } else
  if ((left & 8) != 0 && (middle & 8) != 0) {
    r = right | 16;
  } else
  if ((left & 8) != 0) {
    r = middle | 8;
  } else {
    r = left;
  }
  return r;
}


int lzc24(_FP_Word m) {
  Byte lz1_b, lz1_a, lz1_9, lz1_8, lz1_7, lz1_6;
  Byte lz1_5, lz1_4, lz1_3, lz1_2, lz1_1, lz1_0;
  Byte lz2_5, lz2_4, lz2_3, lz2_2, lz2_1, lz2_0;
  Byte lz3_2, lz3_1, lz3_0;
  Byte lz4_0;

  /* stage 1: encode 2 bits */
  /* representing the LZC of 2 bits */
  lz1_b = encode2((m >> 22) & 0x03);
  lz1_a = encode2((m >> 20) & 0x03);
  lz1_9 = encode2((m >> 18) & 0x03);
  lz1_8 = encode2((m >> 16) & 0x03);
  lz1_7 = encode2((m >> 14) & 0x03);
  lz1_6 = encode2((m >> 12) & 0x03);
  lz1_5 = encode2((m >> 10) & 0x03);
  lz1_4 = encode2((m >>  8) & 0x03);
  lz1_3 = encode2((m >>  6) & 0x03);
  lz1_2 = encode2((m >>  4) & 0x03);
  lz1_1 = encode2((m >>  2) & 0x03);
  lz1_0 = encode2((m >>  0) & 0x03);
  /* stage 2: combine 2+2 bits to 3 bits */
  /* representing the LZC of 4 bits */
  lz2_5 = combine2(lz1_b, lz1_a);
  lz2_4 = combine2(lz1_9, lz1_8);
  lz2_3 = combine2(lz1_7, lz1_6);
  lz2_2 = combine2(lz1_5, lz1_4);
  lz2_1 = combine2(lz1_3, lz1_2);
  lz2_0 = combine2(lz1_1, lz1_0);
  /* stage 3: combine 3+3 bits to 4 bits */
  /* representing the LZC of 8 bits */
  lz3_2 = combine3(lz2_5, lz2_4);
  lz3_1 = combine3(lz2_3, lz2_2);
  lz3_0 = combine3(lz2_1, lz2_0);
  /* stage 4: combine 4+4+4 bits to 5 bits */
  /* representing the LZC of 24 bits */
  lz4_0 = combine4(lz3_2, lz3_1, lz3_0);
  return lz4_0;
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
 * floating-point square root function, implementation
 */


#define DEVELOPING	0

#if DEVELOPING
_FP_Word fake_fpSqr(_FP_Word x) {
  extern _FP_Word Flags;
  extern _FP_Word Flags_ref;
  extern _FP_Word fpSqr_ref(_FP_Word x);
  _FP_Word Flags_save;
  _FP_Word z;

  Flags_save = Flags_ref;
  Flags_ref = 0;
  z = fpSqr_ref(x);
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


_FP_Word fpSqr(_FP_Word x) {
  _FP_Word sx, ex, fx;
  _FP_Word ez, fz;
  Bool nx;
  _FP_Word mx;
  int lx;
  _FP_Word mxnorm;
  int exnorm;
  _FP_Word mxshift;
  _FP_Word fq, fr;
  _FP_Word z;
  Bool round, sticky, odd, incr;

  if (debug) {
    printf("------------------------------------------------\n");
    show("x", x);
    printf("\n");
  }
  sx = _FP_SGN(x);
  ex = _FP_EXP(x);
  fx = _FP_FRC(x);
  if (debug) {
    printf("x: sx = %d, ex = %3d, fx = 0x%06X\n", sx, ex, fx);
  }
  if (IS_ZERO(ex, fx)) {
    z = _FP_FLT(sx, 0, 0);
  } else
  if (IS_INF(ex, fx) && sx == 0) {
    z = genINF;
  } else
  if (sx == 1 && !IS_NAN(ex, fx)) {
    z = genNAN;
    Flags |= _FP_V_FLAG;
  } else
  if (IS_NAN(ex, fx)) {
    z = x | QUIET_BIT;
    if (!IS_QUIET(fx)) {
      Flags |= _FP_V_FLAG;
    }
  } else {
    if (!IS_NUM(ex, fx) || sx == 1) {
      fprintf(stderr, "FATAL INTERNAL ERROR 1\n");
      exit(1);
    }
    /* stage 1: normalize input */
    nx = IS_NORM(ex);
    mx = (nx << 23) | fx;
    lx = lzc24(mx);
    mxnorm = mx << lx;
    exnorm = ex - 127 + (1 - nx) - lx;
    if (debug) {
      printf("nx = %d, mx = 0x%08X, lx = %d, mxnorm = 0x%08X, exnorm = %d\n",
             nx, mx, lx, mxnorm, exnorm);
    }
    /* stage 2: extract root, compute exponent */
    if (exnorm & 1) {
      mxshift = mxnorm << 2;
    } else {
      mxshift = mxnorm << 1;
    }
    if (debug) {
      printf("mxshift = 0x%08X\n", mxshift);
    }
    ez = (exnorm >> 1) + 127;
    sqr24(mxshift, &fq, &fr);
    if (debug) {
      printf("ez = %d, fq = 0x%08X, fr = 0x%08X\n", ez, fq, fr);
    }
    fz = (fq >> 1) & ~(1 << 23);
    round = ((fq & 1) != 0);
    sticky = (fr != 0);
    odd = ((fz & 1) != 0);
    switch (rounding) {
      case ROUND_NEAR:
        incr = round && (sticky || odd);
        break;
      case ROUND_ZERO:
        incr = false;
        break;
      case ROUND_DOWN:
        incr = false;
        break;
      case ROUND_UP:
        incr = round || sticky;
        break;
      default:
        fprintf(stderr, "FATAL INTERNAL ERROR 3\n");
        exit(1);
    }
    if (debug) {
      printf("round = %d, sticky = %d, odd = %d => incr = %d\n",
             round, sticky, odd, incr);
    }
    z = _FP_FLT(0, ez, fz);
    if (incr) {
      z++;
    }
    if (round || sticky) {
      Flags |= _FP_X_FLAG;
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
 * floating-point square root function, internal reference
 */


#include "softfloat.h"


_FP_Word Flags_ref = 0;


_FP_Word fpSqr_ref(_FP_Word x) {
  float32_t X;
  float32_t Z;

  X.v = x;
  softfloat_detectTininess = softfloat_tininess_beforeRounding;
  switch (rounding) {
    case ROUND_NEAR:
      softfloat_roundingMode = softfloat_round_near_even;
      break;
    case ROUND_ZERO:
      softfloat_roundingMode = softfloat_round_minMag;
      break;
    case ROUND_DOWN:
      softfloat_roundingMode = softfloat_round_min;
      break;
    case ROUND_UP:
      softfloat_roundingMode = softfloat_round_max;
      break;
    default:
      fprintf(stderr, "FATAL INTERNAL ERROR 4\n");
      exit(1);
  }
  softfloat_exceptionFlags = 0;
  Z = f32_sqrt(X);
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
  int i;
  _FP_Word x;
  _FP_Word z, r;
  _FP_Union u;
  Bool error;

  /* [1.0, 20.0] */
  printf("------------------------------------------------\n");
  printf("interval = [1.0, 20.0]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    u.f = (float) i;
    x = u.w;
    Flags = 0;
    z = fpSqr(x);
    Flags_ref = 0;
    r = fpSqr_ref(x);
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
  /* [0.10, 2.00] */
  printf("------------------------------------------------\n");
  printf("interval = [0.10, 2.00]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    u.f = (float) i / (float) 10.0;
    x = u.w;
    Flags = 0;
    z = fpSqr(x);
    Flags_ref = 0;
    r = fpSqr_ref(x);
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
 * check the floating-point operation with selected values
 */


typedef struct {
  _FP_Word x;
} _FP_Single;


_FP_Single singles[] = {
  /*
   * special values
   */
  /* { zero, inf } */
  { 0x00000000 },
  { 0x7F800000 },
  { 0x80000000 },
  { 0xFF800000 },
  /* { sNan, qNan } */
  { 0x7F800005 },
  { 0x7FC00005 },
  { 0xFF800005 },
  { 0xFFC00005 },
  /*
   * exponents
   */
  { 0x3E000000 },
  { 0x3E800000 },
  { 0x3F000000 },
  { 0x3F800000 },
  { 0x40000000 },
  { 0x40800000 },
  { 0x41000000 },
  { 0x41800000 },
  { 0x42000000 },
  { 0x42800000 },
  { 0x43000000 },
  { 0x43800000 },
  { 0x44000000 },
  { 0x44800000 },
  { 0x45000000 },
  { 0x45800000 },
  { 0x46000000 },
  { 0x46800000 },
  { 0x47000000 },
  { 0x47800000 },
  { 0x48000000 },
  { 0x48800000 },
  { 0x49000000 },
  { 0x49800000 },
  { 0x4A000000 },
  { 0x4A800000 },
  { 0x4B000000 },
  { 0x4B800000 },
  { 0x4C000000 },
  { 0x4C800000 },
  { 0x4D000000 },
  { 0x4D800000 },
  { 0x4E000000 },
  { 0x4E800000 },
  { 0x4F000000 },
  { 0x4F800000 },
  { 0x50000000 },
  { 0x50800000 },
  /*
   * other tests
   */
  { 0x4F951295 },
  { 0x4E000000 },
  { 0x45800000 },
  { 0xC5800000 },
  { 0x4F000000 },
  { 0xCF000000 },
};

int numSingles = sizeof(singles)/sizeof(singles[0]);


void check_selected(void) {
  _FP_Word x;
  _FP_Word z, r;
  int i;
  Bool error;

  error = false;
  for (i = 0; i < numSingles; i++) {
    x = singles[i].x;
    Flags = 0;
    z = fpSqr(x);
    Flags_ref = 0;
    r = fpSqr_ref(x);
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


#define SKIP_MASK	0x00003F00


void check_intervals(void) {
  _FP_Word x;
  _FP_Word z, r;
  long count, errors;

  printf("------------------------------------------------\n");
  printf("Part 1: full small interval [0x3FABBBBB, 0x3FABCDEF]\n");
  count = 0;
  errors = 0;
  x = 0x3FABBBBB;
  do {
    Flags = 0;
    z = fpSqr(x);
    Flags_ref = 0;
    r = fpSqr_ref(x);
    count++;
    if (!compareEqual(z, r) || Flags != Flags_ref) {
      if (errors == 0) {
        printf("first error: x = 0x%08X\n", x);
        printf("             z = 0x%08X, r = 0x%08X\n", z, r);
      }
      errors++;
    }
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
    Flags = 0;
    z = fpSqr(x);
    Flags_ref = 0;
    r = fpSqr_ref(x);
    count++;
    if (!compareEqual(z, r) || Flags != Flags_ref) {
      if (errors == 0) {
        printf("first error: x = 0x%08X\n", x);
        printf("             z = 0x%08X, r = 0x%08X\n", z, r);
      }
      errors++;
    }
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
    Flags = 0;
    z = fpSqr(x);
    Flags_ref = 0;
    r = fpSqr_ref(x);
    count++;
    if (!compareEqual(z, r) || Flags != Flags_ref) {
      if (errors == 0) {
        printf("first error: x = 0x%08X\n", x);
        printf("             z = 0x%08X, r = 0x%08X\n", z, r);
      }
      errors++;
    }
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
  _FP_Word x;
  _FP_Word z;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    x = strtoul(endptr, &endptr, 16);
    Flags = 0;
    z = fpSqr(x);
    printf("%08X %08X %02X\n", x, z, Flags);
  }
}


/**************************************************************/

/*
 * main program
 */


void usage(char *myself) {
  printf("usage: %s\n"
         "       (-rn | -rz | -rd | -ru)\n"
         "       (-simple | -selected | -intervals | -server)\n"
         "tests: %s -sqr24\n"
         "       %s -lzc24\n",
         myself, myself, myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc == 2 && strcmp(argv[1], "-sqr24") == 0) {
    printf("Check 24-bit square root\n");
    check_sqr24();
    return 0;
  }
  if (argc == 2 && strcmp(argv[1], "-lzc24") == 0) {
    printf("Check 24-bit leading-zero counter\n");
    check_lzc24();
    return 0;
  }
  if (argc != 3) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-rn") == 0) {
    rounding = ROUND_NEAR;
  } else
  if (strcmp(argv[1], "-rz") == 0) {
    rounding = ROUND_ZERO;
  } else
  if (strcmp(argv[1], "-rd") == 0) {
    rounding = ROUND_DOWN;
  } else
  if (strcmp(argv[1], "-ru") == 0) {
    rounding = ROUND_UP;
  } else {
    usage(argv[0]);
  }
  if (strcmp(argv[2], "-simple") == 0) {
    printf("Check simple test cases\n");
    debug = true;
    check_simple();
  } else
  if (strcmp(argv[2], "-selected") == 0) {
    printf("Check selected test cases\n");
    debug = true;
    check_selected();
  } else
  if (strcmp(argv[2], "-intervals") == 0) {
    printf("Check different intervals\n");
    check_intervals();
  } else
  if (strcmp(argv[2], "-server") == 0) {
    server();
  } else {
    usage(argv[0]);
  }
  return 0;
}
