/*
 * exp.c -- implementation and test of exponential function
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


#define REQ_ULP		66	/* requested accuracy in ulp */


/**************************************************************/


#define EDOM		33
#define ERANGE		34


int errno = 0;


/**************************************************************/


void dump(float x) {
  _FP_Union X;

  X.f = x;
  printf("%e = 0x%08X = [%c%02X.%06X]",
         X.f, X.w, _FP_SGN(X.w) ? '-' : '+',
         _FP_EXP(X.w), _FP_FRC(X.w));
}


/**************************************************************/


float fp_floor(float x) {
  _FP_Union X;
  int expX;
  _FP_Word mask;

  X.f = x;
  if ((X.w & 0x7FFFFFFF) == 0) {
    return X.f;
  }
  expX = ((int) _FP_EXP(X.w)) - 127;
  if (expX < 0) {
    if (_FP_SGN(X.w)) {
      X.w = 0xBF800000;
    } else {
      X.w = 0x00000000;
    }
    return X.f;
  }
  if (expX >= 23) {
    if (expX == 128 && _FP_FRC(X.w) != 0) {
      X.w |= 0x00400000;
    }
    return X.f;
  }
  mask = 0x007FFFFF >> expX;
  if ((X.w & mask) == 0) {
    return X.f;
  }
  if (_FP_SGN(X.w)) {
    X.w += (0x00800000 >> expX);
  }
  X.w &= ~mask;
  return X.f;
}


float fp_ldexp(float x, int n) {
  float y;
  _FP_Union M, N;
  float z;

  y = x;
  if (n > 127) {
    M.w = _FP_FLT(0, 127 + 127, 0);
    y *= M.f;
    n -= 127;
    if (n > 127) {
      y *= M.f;
      n -= 127;
      if (n > 127) {
        n = 127;
      }
    }
  } else
  if (n < -126) {
    M.w = _FP_FLT(0, -126 + 127, 0);
    N.w = _FP_FLT(0, 24 + 127, 0);
    y *= M.f * N.f;
    n += 126 - 24;
    if (n < -126) {
      y *= M.f * N.f;
      n += 126 - 24;
      if (n < -126) {
        n = -126;
      }
    }
  }
  M.w = _FP_FLT(0, n + 127, 0);
  z = y * M.f;
  return z;
}


/**************************************************************/


int debug = 0;


static float maxf = 10000;
static float log2e = 1.4426950408889634073599247;
static float p0 = 0.2080384346694663001443843411e7;
static float p1 = 0.3028697169744036299076048876e5;
static float p2 = 0.6061485330061080841615584556e2;
static float q0 = 0.6002720360238832528230907598e7;
static float q1 = 0.3277251518082914423057964422e6;
static float q2 = 0.1749287689093076403844945335e4;
static float sqrt2 = 1.4142135623730950488016887;


float fp_exp(float x) {
  int ent;
  float fract, xsq;
  float temp1, temp2;

  if (x == 0.0) {
    return 1.0;
  }
  if (x < -maxf) {
    return 0.0;
  }
  if (x > maxf) {
    errno = ERANGE;
    return HUGE_VAL;
  }
  x *= log2e;
  ent = fp_floor(x);
  fract = (x - ent) - 0.5;
  xsq = fract * fract;
  temp1 = ((p2 * xsq + p1) * xsq + p0) * fract;
  temp2 = ((1.0 * xsq + q2) * xsq + q1) * xsq + q0;
  return fp_ldexp(sqrt2 * (temp2 + temp1) / (temp2 - temp1), ent);
}


/**************************************************************/


void test_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_exp(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = exp(x);
  printf("r = ");
  dump(r);
  printf("\n");
}


/**************************************************************/


/*
 * This function returns pseudo random numbers uniformly
 * distributed over (0,1).
 */
float ran(void) {
  return (float) rand() / (float) RAND_MAX;
}


/*
 * This function returns pseudo random numbers logarithmically
 * distributed over (1,exp(x)). Thus a*randl(ln(b/a)) is
 * logarithmically distributed in (a,b).
 */
float randl(float x) {
  return exp(x * ran());
}


/**************************************************************/


void test_many(int count, float lbound, float ubound, int maybeNeg) {
  int i;
  int below, equal, above, within, ulp;
  float x, z, r;
  _FP_Union Z, R;

  printf("absolute operand interval: (%e,%e)\n", lbound, ubound);
  printf("operands may%sbe negative\n", maybeNeg ? " " : " not ");
  below = 0;
  equal = 0;
  above = 0;
  within = 0;
  ulp = REQ_ULP;
  for (i = 1; i <= count; i++) {
    if (i % 100 == 0) {
      printf("\rnumber of tests: %d", i);
      fflush(stdout);
    }
    x = lbound * randl(log(ubound / lbound));
    if (maybeNeg) {
      if (rand() & 0x400) {
        x = -x;
      }
    }
    z = fp_exp(x);
    r = exp(x);
    if (z < r) {
      below++;
    } else
    if (z > r) {
      above++;
    } else {
      equal++;
    }
    Z.f = z;
    R.f = r;
    if (_FP_ABS(_FP_DELTA(Z, R)) <= ulp) {
      within++;
    } else {
      printf("++++++++++++++++++++\n");
      printf("x = ");
      dump(x);
      printf("\n");
      debug = 1;
      z = fp_exp(x);
      debug = 0;
      printf("z = ");
      dump(z);
      printf("\n");
      printf("r = ");
      dump(r);
      printf("\n");
      printf("++++++++++++++++++++\n");
    }
  }
  printf("\n");
  printf("below: %d\n", below);
  printf("equal: %d\n", equal);
  printf("above: %d\n", above);
  printf("within error of %d ulp: %d\n", ulp, within);
}


/**************************************************************/


void testFew(void) {
  printf("------------------------------------------------\n");
  test_single(0.0);
  printf("------------------------------------------------\n");
  test_single(-0.0);
  printf("------------------------------------------------\n");
  test_single(2.3);
  printf("------------------------------------------------\n");
  test_single(-2.3);
  printf("------------------------------------------------\n");
  test_single(2.5);
  printf("------------------------------------------------\n");
  test_single(-2.5);
  printf("------------------------------------------------\n");
  test_single(2.7);
  printf("------------------------------------------------\n");
  test_single(-2.7);
  printf("------------------------------------------------\n");
  test_single(3.0);
  printf("------------------------------------------------\n");
  test_single(-3.0);
  printf("------------------------------------------------\n");
  test_single(1.2345e2);
  printf("------------------------------------------------\n");
  test_single(-1.2345e2);
  printf("------------------------------------------------\n");
  test_single(1.2345e-3);
  printf("------------------------------------------------\n");
  test_single(-1.2345e-3);
  printf("------------------------------------------------\n");
}


/**************************************************************/


void testMany(void) {
  printf("------------------------------------------------\n");
  test_many(10000000, 0.25, 4.0, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0, 1.0e15, 1);
  printf("------------------------------------------------\n");
}


/**************************************************************/


void testAll(int skipSome) {
  int ulp;
  unsigned int errors, i;
  _FP_Union X, Z, R;

  ulp = REQ_ULP;
  errors = 0;
  i = 0;
  do {
    if ((i & 0x0FFFFFFF) == 0) {
      printf("reached test 0x%08X\n", i);
    }
    if (skipSome) {
      while ((i & 0x0003FFC0) != 0x00000000 &&
             (i & 0x0003FFC0) != 0x0003FFC0) {
        i += 0x00000040;
      }
    }
    X.w = i;
    Z.f = fp_exp(X.f);
    R.f = exp(X.f);
    if (_FP_ABS(_FP_DELTA(Z, R)) > ulp) {
      if (errors == 0) {
        printf("first error at X = 0x%08X: Z = 0x%08X, R = 0x%08X\n",
               X.w, Z.w, R.w);
      }
      errors++;
    }
    i++;
  } while (i != 0);
  printf("done, %u errors (not within %d ulp of reference)\n",
         errors, ulp);
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s -few|-many|-most|-all\n", myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-few") == 0) {
    testFew();
  } else
  if (strcmp(argv[1], "-many") == 0) {
    testMany();
  } else
  if (strcmp(argv[1], "-most") == 0) {
    testAll(1);
  } else
  if (strcmp(argv[1], "-all") == 0) {
    testAll(0);
  } else {
    usage(argv[0]);
  }
  return 0;
}
