/*
 * exp.c -- implementation and test of exponential function
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


#define REQ_ULP		18	/* requested accuracy in ulp */


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


static float	p0	= .2080384346694663001443843411e7;
static float	p1	= .3028697169744036299076048876e5;
static float	p2	= .6061485330061080841615584556e2;
static float	q0	= .6002720360238832528230907598e7;
static float	q1	= .3277251518082914423057964422e6;
static float	q2	= .1749287689093076403844945335e4;
static float	log2e	= 1.4426950408889634073599247;
static float	sqrt2	= 1.4142135623730950488016887;
static float	maxf	= 10000;


float fp_exp(float arg) {
  int ent;
  float fract;
  float xsq;
  float temp1;
  float temp2;

  if (arg == 0.0) {
    return 1.0;
  }
  if (arg < -maxf) {
    return 0.0;
  }
  if (arg > maxf) {
    errno = ERANGE;
    return HUGE_VAL;
  }
  arg *= log2e;
  ent = floor(arg);
  fract = (arg - ent) - 0.5;
  xsq = fract * fract;
  temp1 = ((p2 * xsq + p1) * xsq + p0) * fract;
  temp2 = ((1.0 * xsq + q2) * xsq + q1) * xsq + q0;
  return ldexp(sqrt2 * (temp2 + temp1) / (temp2 - temp1), ent);
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
