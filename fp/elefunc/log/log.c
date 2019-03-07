/*
 * log.c -- implementation and test of natural and common logarithm
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


#define REQ_ULP		4	/* requested accuracy in ulp */


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


#define A0	-0.5527074855e+0	/* rational approximation */
#define B0	-0.6632718214e+1	/* Cody & Waite, p. 44 */
#define C0	0.70710678119		/* sqrt(1/2) */
#define C1	0.69335937500		/* 355/512 */
#define C2	-2.1219444005e-4	/* C1 + C2 = ln(2) */
#define C3	0.43429448190		/* log(e) */


int debug = 0;


float fp_log(float x) {
  _FP_Union X;
  int expX;
  _FP_Word frcX;
  _FP_Union F;
  float znum, zden;
  float z, w, r, s;
  float v;

  X.f = x;
  if ((X.w & 0x7FFFFFFF) == 0) {
    errno = EDOM;
    X.w = 0xFF800000;
    return X.f;
  }
  if ((X.w & 0x80000000) != 0) {
    errno = EDOM;
    X.w = 0x7FC00000;
    return X.f;
  }
  expX = ((int) _FP_EXP(X.w)) - 126;
  frcX = _FP_FRC(X.w);
  F.w = _FP_FLT(0, 126, frcX);
  if (debug) {
    printf("f = ");
    dump(F.f);
    printf("\n");
  }
  if (F.f > C0) {
    znum = (F.f - 0.5) - 0.5;
    zden = F.f * 0.5 + 0.5;
    if (debug) {
      printf("f > C0 : znum = %e, zden = %e\n", znum, zden);
    }
  } else {
    expX--;
    znum = F.f - 0.5;
    zden = znum * 0.5 + 0.5;
    if (debug) {
      printf("f <= C0 : znum = %e, zden = %e\n", znum, zden);
    }
  }
  z = znum / zden;
  w = z * z;
  if (debug) {
    printf("z = %e, w = %e\n", z, w);
  }
  r = w * A0 / (w + B0);
  s = z + z * r;
  if (debug) {
    printf("r = %e, s = %e\n", r, s);
  }
  v = (float) expX;
  v = (v * C2 + s) + v * C1;
  return v;
}


float fp_log10(float x) {
  return fp_log(x) * C3;
}


/**************************************************************/


void test_single_1(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_log(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = log(x);
  printf("r = ");
  dump(r);
  printf("\n");
}


void test_single_2(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_log10(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = log10(x);
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


void test_many_1(int count, float lbound, float ubound, int maybeNeg) {
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
    z = fp_log(x);
    r = log(x);
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
      z = fp_log(x);
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


void test_many_2(int count, float lbound, float ubound, int maybeNeg) {
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
    z = fp_log10(x);
    r = log10(x);
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
      z = fp_log10(x);
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


int main(void) {
  printf("------------------------------------------------\n");
  test_single_1(0.0);
  printf("------------------------------------------------\n");
  test_single_1(-0.0);
  printf("------------------------------------------------\n");
  test_single_1(-4.0);
  printf("------------------------------------------------\n");
  test_single_1(1.0);
  printf("------------------------------------------------\n");
  test_single_1(0.5);
  printf("------------------------------------------------\n");
  test_single_1(2.0);
  printf("------------------------------------------------\n");
  test_single_1(10.0);
  printf("------------------------------------------------\n");
  test_single_1(100.0);
  printf("------------------------------------------------\n");
  test_many_1(10000000, 0.25, 4.0, 0);
  printf("------------------------------------------------\n");
  test_many_1(10000000, 1.0e-15, 1.0, 0);
  printf("------------------------------------------------\n");
  test_many_1(10000000, 1.0, 1.0e15, 0);
  printf("------------------------------------------------\n");
  printf("------------------------------------------------\n");
  test_single_2(0.0);
  printf("------------------------------------------------\n");
  test_single_2(-0.0);
  printf("------------------------------------------------\n");
  test_single_2(-4.0);
  printf("------------------------------------------------\n");
  test_single_2(1.0);
  printf("------------------------------------------------\n");
  test_single_2(0.5);
  printf("------------------------------------------------\n");
  test_single_2(2.0);
  printf("------------------------------------------------\n");
  test_single_2(10.0);
  printf("------------------------------------------------\n");
  test_single_2(100.0);
  printf("------------------------------------------------\n");
  test_many_2(10000000, 0.25, 4.0, 0);
  printf("------------------------------------------------\n");
  test_many_2(10000000, 1.0e-15, 1.0, 0);
  printf("------------------------------------------------\n");
  test_many_2(10000000, 1.0, 1.0e15, 0);
  printf("------------------------------------------------\n");
  return 0;
}
