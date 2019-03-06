/*
 * sqrt.c -- implementation and test of floating-point square root
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


#define REQ_ULP		2	/* requested accuracy in ulp */


/**************************************************************/


#define EDOM		33


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


#define CONST_1		0.59466991411
#define CONST_2		0.41421356237
#define CONST_3		0.70710678119	/* 1/sqrt(2) */


int debug = 0;


float fp_sqrt(float x) {
  _FP_Union X, Z;
  _FP_Word expX, expZ;
  _FP_Word frcX, frcZ;
  _FP_Union A1, A2;

  X.f = x;
  if ((X.w & 0x7FFFFFFF) == 0) {
    return X.f;
  }
  if ((X.w & 0x80000000) != 0) {
    errno = EDOM;
    X.w = 0xFFC00000;
    return X.f;
  }
  expX = _FP_EXP(X.w);
  frcX = _FP_FRC(X.w);
  A1.w = _FP_FLT(0, 127, frcX);
  if (debug) {
    printf("f=");
    dump(A1.f);
    printf("\n");
  }
  A2.f = CONST_1 + CONST_2 * A1.f;
  if (debug) {
    printf("y0=");
    dump(A2.f);
    printf("\n");
  }
  A2.f = 0.5 * (A2.f + A1.f / A2.f);
  if (debug) {
    printf("y1=");
    dump(A2.f);
    printf("\n");
  }
  A2.f = 0.5 * (A2.f + A1.f / A2.f);
  if (debug) {
    printf("y2=");
    dump(A2.f);
    printf("\n");
  }
  expZ = expX - 127;
  if (expZ & 1) {
    A2.f *= CONST_3;
    if (debug) {
      printf("corrected y2=");
      dump(A2.f);
      printf("\n");
    }
    expZ++;
  }
  expZ = _FP_EXP(A2.w) + expZ / 2;
  frcZ = _FP_FRC(A2.w);
  Z.w = _FP_FLT(0, expZ, frcZ);
  return Z.f;
}


/**************************************************************/


void test_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_sqrt(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = sqrt(x);
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
    z = fp_sqrt(x);
    r = sqrt(x);
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
      printf("x=");
      dump(x);
      printf("\n");
      debug = 1;
      z = fp_sqrt(x);
      debug = 0;
      printf("z=");
      dump(z);
      printf("\n");
      printf("r=");
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
  test_single(1.0);
  printf("------------------------------------------------\n");
  test_single(0.5);
  printf("------------------------------------------------\n");
  test_single(2.0);
  printf("------------------------------------------------\n");
  test_single(4.0);
  printf("------------------------------------------------\n");
  test_single(8.0);
  printf("------------------------------------------------\n");
  test_single(1.234e2);
  printf("------------------------------------------------\n");
  test_single(1.234e-3);
  printf("------------------------------------------------\n");
  test_many(10000000, 0.5, 2.0, 0);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0, 0);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0, 1.0e15, 0);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0e15, 0);
  printf("------------------------------------------------\n");
  return 0;
}
