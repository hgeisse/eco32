/*
 * floor.c -- implementation and test of floor function
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


float fp_floor(float x) {
  _FP_Union X;
  int expX;
  _FP_Word mask;

  X.f = x;
  if ((X.w & 0x7FFFFFFF) == 0) {
    return X.f;
  }
  expX = ((int) _FP_EXP(X.w)) - 127;
  if (debug) {
    printf("expX = %d\n", expX);
  }
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


/**************************************************************/


void test_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_floor(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = floor(x);
  printf("r = ");
  dump(r);
  printf("\n");
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


#define SKIP_MASK	0x0000FF00


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
      if ((i & SKIP_MASK) != 0x00000000 &&
          (i & SKIP_MASK) != SKIP_MASK) {
        i |= SKIP_MASK;
      }
    }
    X.w = i;
    Z.f = fp_floor(X.f);
    R.f = floor(X.f);
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
