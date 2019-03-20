/*
 * fabs.c -- implementation and test of absolute value
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


float fp_fabs(float x) {
  _FP_Union X;

  X.f = x;
  X.w &= ~0x80000000;
  return X.f;
}


/**************************************************************/


void test_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_fabs(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = fabs(x);
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
    Z.f = fp_fabs(X.f);
    R.f = fabs(X.f);
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
