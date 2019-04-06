/*
 * sin.c -- implementation and test of sine
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


#define REQ_ULP		4096	/* requested accuracy in ulp */


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


static float twoopi	= 0.63661977236758134308;
static float p0		=  .1357884097877375669092680e8;
static float p1		= -.4942908100902844161158627e7;
static float p2		=  .4401030535375266501944918e6;
static float p3		= -.1384727249982452873054457e5;
static float p4		=  .1459688406665768722226959e3;
static float q0		=  .8644558652922534429915149e7;
static float q1		=  .4081792252343299749395779e6;
static float q2		=  .9463096101538208180571257e4;
static float q3		=  .1326534908786136358911494e3;


static float float_modf(float x, float *ip) {
  double aux, res;

  res = modf((double) x, &aux);
  *ip = (float) aux;
  return (float) res;
}


float fp_sin(float arg) {
  int quad;
  float x;
  float y;
  float e;
  float f;
  int k;
  float ysq;
  float temp1;
  float temp2;

  quad = 0;
  x = arg;
  if (x < 0) {
    x = -x;
    quad += 2;
  }
  x *= twoopi;
  if (x > 32764) {
    y = float_modf(x, &e);
    e += quad;
    float_modf(0.25 * e, &f);
    quad = e - 4 * f;
  } else {
    k = x;
    y = x - k;
    quad = (quad + k) & 3;
  }
  if (quad & 1) {
    y = 1 - y;
  }
  if (quad > 1) {
    y = -y;
  }
  ysq = y * y;
  temp1 = ((((p4 * ysq + p3) * ysq + p2) * ysq + p1) * ysq + p0) * y;
  temp2 = ((((ysq + q3) * ysq + q2) * ysq + q1) * ysq + q0);
  return temp1 / temp2;
}


/**************************************************************/


void test_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_sin(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = sin(x);
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
    z = fp_sin(x);
    r = sin(x);
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
    Z.f = fp_sin(X.f);
    R.f = sin(X.f);
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
