/*
 * log.c -- implementation and test of natural and common logarithm
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


static float	ln2	= 0.693147180559945309e0;
static float	ln10	= 2.302585092994045684;
static float	sqrto2	= 0.707106781186547524e0;
static float	p0	= -.240139179559210510e2;
static float	p1	= 0.309572928215376501e2;
static float	p2	= -.963769093368686593e1;
static float	p3	= 0.421087371217979714e0;
static float	q0	= -.120069589779605255e2;
static float	q1	= 0.194809660700889731e2;
static float	q2	= -.891110902798312337e1;


float fp_log(float arg) {
  float x;
  int exp;
  float z;
  float zsq;
  float temp;

  if (arg <= 0.0) {
    errno = EDOM;
    return -HUGE_VAL;
  }
  x = frexp(arg, &exp);
  while (x < 0.5) {
    x *= 2;
    exp--;
  }
  if (x < sqrto2) {
    x *= 2;
    exp--;
  }
  z = (x - 1) / (x + 1);
  zsq = z * z;
  temp = ((p3 * zsq + p2) * zsq + p1) * zsq + p0;
  temp = temp / (((1.0 * zsq + q2) * zsq + q1) * zsq + q0);
  temp = temp * z + exp * ln2;
  return temp;
}


float fp_log10(float arg) {
  return log(arg) / ln10;
}


/**************************************************************/


void test_single(float x) {
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


/**************************************************************/


void testFew(void) {
  printf("------------------------------------------------\n");
  test_single(1.0);
  printf("------------------------------------------------\n");
  test_single(2.718281828);
  printf("------------------------------------------------\n");
  test_single(0.367879441);
  printf("------------------------------------------------\n");
  test_single(4.0);
  printf("------------------------------------------------\n");
  test_single(0.25);
  printf("------------------------------------------------\n");
  test_single(8.0);
  printf("------------------------------------------------\n");
  test_single(0.125);
  printf("------------------------------------------------\n");
  test_single(1.2345e2);
  printf("------------------------------------------------\n");
  test_single(1.2345e-3);
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
  test_many(10000000, 0.25, 4.0, 0);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0, 0);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0, 1.0e15, 0);
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
    Z.f = fp_log(X.f);
    R.f = log(X.f);
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
