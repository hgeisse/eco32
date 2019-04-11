/*
 * atan.c -- implementation and test of arctangent
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


static float sq2p1	= 2.414213562373095048802e0;
static float sq2m1	=  .414213562373095048802e0;
static float pio2	= 1.570796326794896619231e0;
static float pio4	=  .785398163397448309615e0;
static float p4		=  .161536412982230228262e2;
static float p3		=  .26842548195503973794141e3;
static float p2		=  .11530293515404850115428136e4;
static float p1		=  .178040631643319697105464587e4;
static float p0		=  .89678597403663861959987488e3;
static float q4		=  .5895697050844462222791e2;
static float q3		=  .536265374031215315104235e3;
static float q2		=  .16667838148816337184521798e4;
static float q1		=  .207933497444540981287275926e4;
static float q0		=  .89678597403663861962481162e3;


static float fp_xatan(float arg) {
  float asq;
  float value;

  asq = arg * arg;
  value = (((p4 * asq + p3) * asq + p2) * asq + p1) * asq + p0;
  value /= ((((asq + q4) * asq + q3) * asq + q2) * asq + q1) * asq + q0;
  return value * arg;
}


static float fp_satan(float arg) {
  if (arg < sq2m1) {
    return fp_xatan(arg);
  }
  if (arg > sq2p1) {
    return pio2 - fp_xatan(1.0 / arg);
  }
  return pio4 + fp_xatan((arg - 1.0) / (arg + 1.0));
}


float fp_atan(float arg) {
  if (arg > 0) {
    return fp_satan(arg);
  } else {
    return -fp_satan(-arg);
  }
}


float fp_atan2(float arg1, float arg2) {
  if ((arg1 + arg2) == arg1) {
    if (arg1 >= 0.0) {
      return pio2;
    } else {
      return -pio2;
    }
  }
  if (arg2 < 0.0) {
    if (arg1 >= 0.0) {
      return pio2 + pio2 - fp_satan(-arg1 / arg2);
    } else {
      return -pio2 - pio2 + fp_satan(arg1 / arg2);
    }
  }
  if (arg1 > 0) {
    return fp_satan(arg1 / arg2);
  } else {
    return -fp_satan(-arg1 / arg2);
  }
}


/**************************************************************/


void test_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_atan(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = atan(x);
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
    z = fp_atan(x);
    r = atan(x);
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
    Z.f = fp_atan(X.f);
    R.f = atan(X.f);
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
