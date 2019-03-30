/*
 * frexp.c -- implementation and test of frexp function
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


#define REQ_ULP		0	/* requested accuracy in ulp */


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


int debug = 0;


float fp_frexp(float x, int *exp) {
  _FP_Union X;
  int expX;
  _FP_Union Y;

  X.f = x;
  *exp = 0;
  if ((X.w & 0x7FFFFFFF) == 0) {
    if (debug) {
      printf("+/-0\n");
    }
    return X.f;
  }
  expX = ((int) _FP_EXP(X.w)) - 126;
  if (expX == 129) {
    if (debug) {
      printf("inf or nan\n");
    }
    if (_FP_FRC(X.w) != 0) {
      X.w |= 0x00400000;
    }
    return X.f;
  }
  if (expX == -126) {
    if (debug) {
      printf("subnormal\n");
    }
    Y.w = 0x4C000000;
    X.f *= Y.f;
    *exp = -25;
    expX = ((int) _FP_EXP(X.w)) - 126;
  }
  if (debug) {
    printf("(now) normal\n");
  }
  *exp += expX;
  Y.w = _FP_FLT(_FP_SGN(X.w), 126, _FP_FRC(X.w));
  return Y.f;
}


/**************************************************************/


void test_single(float x) {
  float z, r;
  int ze, re;

  printf("x = ");
  dump(x);
  printf("\n");
  ze = 0;
  z = fp_frexp(x, &ze);
  printf("z = ");
  dump(z);
  printf(", exp = %d", ze);
  printf("\n");
  re = 0;
  r = frexp(x, &re);
  printf("r = ");
  dump(r);
  printf(", exp = %d", re);
  printf("\n");
}


/**************************************************************/


void testFew(void) {
  printf("------------------------------------------------\n");
  test_single(0.0);
  printf("------------------------------------------------\n");
  test_single(-0.0);
  printf("------------------------------------------------\n");
  test_single(0.5);
  printf("------------------------------------------------\n");
  test_single(-0.5);
  printf("------------------------------------------------\n");
  test_single(3.3);
  printf("------------------------------------------------\n");
  test_single(-3.3);
  printf("------------------------------------------------\n");
  test_single(1.234e2);
  printf("------------------------------------------------\n");
  test_single(-1.234e2);
  printf("------------------------------------------------\n");
  test_single(1.234e-3);
  printf("------------------------------------------------\n");
  test_single(-1.234e-3);
  printf("------------------------------------------------\n");
}


/**************************************************************/


#define SKIP_MASK	0x0000FF00


void testAll(int skipSome) {
  int ulp;
  unsigned int errors, i;
  int ze, re;
  _FP_Union X, Z, R;

  ulp = REQ_ULP;
  errors = 0;
  i = 0;
  do {
    if ((i & 0x0FFFFFFF) == 0) {
      printf("reached test 0x%08X\n", i);
    }
    if (skipSome) {
      if ((i & SKIP_MASK) != 0x00000000 &&
          (i & SKIP_MASK) != SKIP_MASK) {
        i |= SKIP_MASK;
      }
    }
    X.w = i;
    ze = 0;
    Z.f = fp_frexp(X.f, &ze);
    re = 0;
    R.f = frexp(X.f, &re);
    if (_FP_ABS(_FP_DELTA(Z, R)) > ulp || ze != re) {
      if (errors == 0) {
        printf("first error at X = 0x%08X: Z = 0x%08X, R = 0x%08X\n",
               X.w, Z.w, R.w);
        printf("                               exp(Z) = %d, exp(R) = %d\n",
               ze, re);
      }
      errors++;
    }
    i++;
  } while (i != 0);
  printf("done, %u errors (not within %d ulp of reference or exp error)\n",
         errors, ulp);
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s -few|-most|-all\n", myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-few") == 0) {
    testFew();
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
