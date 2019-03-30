/*
 * pow.c -- implementation and test of power function x^y
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


#define REQ_ULP		1	/* requested accuracy in ulp */


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


float fp_pow(float arg1, float arg2) {
  int n;
  float temp;

  if (arg1 <= 0.0) {
    if (arg1 == 0.0) {
      if (arg2 <= 0.0) {
        errno = EDOM;
      }
      return 0.0;
    }
    n = arg2;
    if (n != arg2) {
      errno = EDOM;
      return 0.0;
    }
    temp = exp(arg2 * log(-arg1));
    if (n & 1) {
      temp = -temp;
    }
    return temp;
  }
  return exp(arg2 * log(arg1));
}


/**************************************************************/


void test_single(float x, float y) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  printf("y = ");
  dump(y);
  printf("\n");
  z = fp_pow(x, y);
  printf("z = ");
  dump(z);
  printf("\n");
  r = pow(x, y);
  printf("r = ");
  dump(r);
  printf("\n");
}


/**************************************************************/


void testFew(void) {
  printf("------------------------------------------------\n");
  test_single(0.0, 1.2345);
  printf("------------------------------------------------\n");
  test_single(-1.2345, 1.0);
  printf("------------------------------------------------\n");
  test_single(-1.2345, 2.0);
  printf("------------------------------------------------\n");
  test_single(-1.2345, 3.0);
  printf("------------------------------------------------\n");
  test_single(1.2345, 1.0);
  printf("------------------------------------------------\n");
  test_single(2.0, 2.0);
  printf("------------------------------------------------\n");
  test_single(2.0, 0.5);
  printf("------------------------------------------------\n");
  test_single(2.0, 3.0);
  printf("------------------------------------------------\n");
  test_single(9.0, 0.5);
  printf("------------------------------------------------\n");
  test_single(2.0, -2.0);
  printf("------------------------------------------------\n");
  test_single(2.0, -0.5);
  printf("------------------------------------------------\n");
  test_single(2.0, -3.0);
  printf("------------------------------------------------\n");
  test_single(9.0, -0.5);
  printf("------------------------------------------------\n");
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


void test_many(int count,
               float lbound_x, float ubound_x, int maybeNeg_x,
               float lbound_y, float ubound_y, int maybeNeg_y) {
  int i;
  int below, equal, above, within, ulp;
  float x, y, z, r;
  _FP_Union Z, R;

  printf("absolute operand x interval: (%e,%e)\n", lbound_x, ubound_x);
  printf("operand x may%sbe negative\n", maybeNeg_x ? " " : " not ");
  printf("absolute operand y interval: (%e,%e)\n", lbound_y, ubound_y);
  printf("operand y may%sbe negative\n", maybeNeg_y ? " " : " not ");
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
    x = lbound_x * randl(log(ubound_x / lbound_x));
    if (maybeNeg_x) {
      if (rand() & 0x400) {
        x = -x;
      }
    }
    y = lbound_y * randl(log(ubound_y / lbound_y));
    if (maybeNeg_y) {
      if (rand() & 0x400) {
        y = -y;
      }
    }
    z = fp_pow(x, y);
    r = pow(x, y);
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
    }
  }
  printf("\n");
  printf("below: %d\n", below);
  printf("equal: %d\n", equal);
  printf("above: %d\n", above);
  printf("within error of %d ulp: %d\n", ulp, within);
}


/**************************************************************/


void testMany(void) {
  printf("------------------------------------------------\n");
  test_many(10000000, 0.25, 4.0, 0, 0.25, 4.0, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0, 0, 0.25, 4.0, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0, 1.0e15, 0, 0.25, 4.0, 1);
  printf("------------------------------------------------\n");
}


/**************************************************************/


#define SKIP_MASK	0x001FFFFC


void testAll(int skipSome) {
  int ulp;
  unsigned int errors, i, j;
  _FP_Union X, Y, Z, R;

  ulp = REQ_ULP;
  errors = 0;
  i = 0;
  do {
    if ((i & 0x0FFFFFFF) == 0) {
      printf("reached test loop 0x%08X\n", i);
    }
    if (skipSome) {
      if ((i & SKIP_MASK) != 0x00000000 &&
          (i & SKIP_MASK) != SKIP_MASK) {
        i |= SKIP_MASK;
      }
    }
    j = 0;
    do {
      if (skipSome) {
        if ((j & SKIP_MASK) != 0x00000000 &&
            (j & SKIP_MASK) != SKIP_MASK) {
          j |= SKIP_MASK;
        }
      }
      X.w = i;
      Y.w = j;
      Z.f = fp_pow(X.f, Y.f);
      R.f = pow(X.f, Y.f);
      if (_FP_ABS(_FP_DELTA(Z, R)) > ulp) {
        if (errors == 0) {
          printf("first error at (X,Y) = (0x%08X,0x%08X): ", X.w, Y.w);
          printf("Z = 0x%08X, R = 0x%08X\n", Z.w, R.w);
        }
        errors++;
      }
      j++;
    } while (j != 0);
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
