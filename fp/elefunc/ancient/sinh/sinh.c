/*
 * sinh.c -- implementation and test of hyperbolic sine
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


#define REQ_ULP		3	/* requested accuracy in ulp */


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


static float p0  = -0.6307673640497716991184787251e+6;
static float p1  = -0.8991272022039509355398013511e+5;
static float p2  = -0.2894211355989563807284660366e+4;
static float p3  = -0.2630563213397497062819489e+2;
static float q0  = -0.6307673640497716991212077277e+6;
static float q1   = 0.1521517378790019070696485176e+5;
static float q2  = -0.173678953558233699533450911e+3;


float fp_sinh(float arg) {
  int sign;
  float temp;
  float argsq;

  if (arg < 0) {
    arg = -arg;
    sign = -1;
  } else {
    sign = 1;
  }
  if (arg > 21.0) {
    temp = exp(arg) / 2;
    if (sign > 0) {
      return temp;
    } else {
      return -temp;
    }
  }
  if (arg > 0.5) {
    return sign * (exp(arg) - exp(-arg)) / 2;
  }
  argsq = arg * arg;
  temp = (((p3 * argsq + p2) * argsq + p1) * argsq + p0) * arg;
  temp /= (((argsq + q2) * argsq + q1) * argsq + q0);
  return sign * temp;
}


/**************************************************************/


void test_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_sinh(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = sinh(x);
  printf("r = ");
  dump(r);
  printf("\n");
}


/**************************************************************/


void testFew(void) {
  printf("------------------------------------------------\n");
  test_single(0.0);
  printf("------------------------------------------------\n");
  test_single(1.0);
  printf("------------------------------------------------\n");
  test_single(-1.0);
  printf("------------------------------------------------\n");
  test_single(2.0);
  printf("------------------------------------------------\n");
  test_single(-2.0);
  printf("------------------------------------------------\n");
  test_single(1.2345);
  printf("------------------------------------------------\n");
  test_single(-1.2345);
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


void testSpecial(void) {
  _FP_Union X;

  printf("------------------------------------------------\n");
  X.w = 0x00000000;
  test_single(X.f);
  printf("------------------------------------------------\n");
  X.w = 0x80000000;
  test_single(X.f);
  printf("------------------------------------------------\n");
  X.w = 0xC0A00000;
  test_single(X.f);
  printf("------------------------------------------------\n");
  X.w = 0x7F800000;
  test_single(X.f);
  printf("------------------------------------------------\n");
  X.w = 0xFF800000;
  test_single(X.f);
  printf("------------------------------------------------\n");
  X.w = 0x7F800005;
  test_single(X.f);
  printf("------------------------------------------------\n");
  X.w = 0xFF800005;
  test_single(X.f);
  printf("------------------------------------------------\n");
  X.w = 0x7FC00005;
  test_single(X.f);
  printf("------------------------------------------------\n");
  X.w = 0xFFC00005;
  test_single(X.f);
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
    z = fp_sinh(x);
    r = sinh(x);
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
  test_many(10000000, 0.25, 4.0, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0, 20.0, 1);
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
    Z.f = fp_sinh(X.f);
    R.f = sinh(X.f);
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
  printf("usage: %s -few|-special|-many|-most|-all\n", myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-few") == 0) {
    testFew();
  } else
  if (strcmp(argv[1], "-special") == 0) {
    testSpecial();
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
