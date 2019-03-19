/*
 * ci2f.c -- implementation and test of integer-to-float conversion
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


_FP_Word nlz32(_FP_Word X) {
  _FP_Word Z;

  Z = 0;
  if (X == 0) {
    return 32;
  }
  if (X <= 0x0000FFFF) {
    Z += 16;
    X <<= 16;
  }
  if (X <= 0x00FFFFFF) {
    Z += 8;
    X <<= 8;
  }
  if (X <= 0x0FFFFFFF) {
    Z += 4;
    X <<= 4;
  }
  if (X <= 0x3FFFFFFF) {
    Z += 2;
    X <<= 2;
  }
  if (X <= 0x7FFFFFFF) {
    Z += 1;
  }
  return Z;
}


/**************************************************************/


int debug = 0;


float fp_ci2f(int x) {
  _FP_Union Z;
  _FP_Word signZ;
  int n;
  _FP_Word frcZ;
  int r, s;

  if (x == 0x00000000) {
    Z.w = 0x00000000;
    return Z.f;
  }
  if (x < 0) {
    x = -x;
    signZ = 0x80000000;
  } else {
    signZ = 0x00000000;
  }
  n = nlz32(x);
  if (debug) {
    printf("n = %d\n", n);
  }
  if (n >= 8) {
    frcZ = x << (n - 8);
    r = 0;
    s = 0;
  } else {
    frcZ = x >> (8 - n);
    r = (x & (0x00000001 << (7 - n))) != 0;
    s = (x & ((0x00000001 << (7 - n)) - 1)) != 0;
  }
  if (debug) {
    printf("frcZ = 0x%08X, r = %d, s = %d\n", frcZ, r, s);
  }
  Z.w = _FP_FLT(signZ, 158 - n, frcZ & 0x007FFFFF) + (r & (frcZ | s));
  return Z.f;
}


/**************************************************************/

/* reference */


float ci2f(int x) {
  return (float) x;
}


/**************************************************************/


void test_single(int x) {
  float z, r;

  printf("x = 0x%08X", x);
  printf("\n");
  z = fp_ci2f(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = ci2f(x);
  printf("r = ");
  dump(r);
  printf("\n");
}


/**************************************************************/


void testFew(void) {
  printf("------------------------------------------------\n");
  test_single(0);
  printf("------------------------------------------------\n");
  test_single(-0);
  printf("------------------------------------------------\n");
  test_single(2);
  printf("------------------------------------------------\n");
  test_single(-2);
  printf("------------------------------------------------\n");
  test_single(127);
  printf("------------------------------------------------\n");
  test_single(-127);
  printf("------------------------------------------------\n");
  test_single(0x12345678);
  printf("------------------------------------------------\n");
  test_single(-0x12345678);
  printf("------------------------------------------------\n");
  test_single(0x003FFFFF);
  printf("------------------------------------------------\n");
  test_single(0x00400000);
  printf("------------------------------------------------\n");
  test_single(0x007FFFFF);
  printf("------------------------------------------------\n");
  test_single(0x00800000);
  printf("------------------------------------------------\n");
  test_single(0x00FFFFFF);
  printf("------------------------------------------------\n");
  test_single(0x01000000);
  printf("------------------------------------------------\n");
  test_single(0x01FFFFFF);
  printf("------------------------------------------------\n");
  test_single(0x02000000);
  printf("------------------------------------------------\n");
  test_single(0x01000001);
  printf("------------------------------------------------\n");
  test_single(0x01000002);
  printf("------------------------------------------------\n");
  test_single(0x01000003);
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
    Z.f = fp_ci2f(X.w);
    R.f = ci2f(X.w);
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
