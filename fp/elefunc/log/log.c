/*
 * log.c -- implementation and test of natural and common logarithm
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define SGN(x)		((x) & 0x80000000)
#define EXP(x)		(((x) << 1) >> 24)
#define FRC(x)		((x) & 0x7FFFFF)
#define FLOAT(s,e,f)	(s | (e << 23) | f)

#define ABS(x)		((x) < 0 ? -(x) : (x))
#define DELTA(Z,R)	((int) (Z.w & 0x7FFFFFFF) - (int) (R.w & 0x7FFFFFFF))


typedef unsigned int Word;


typedef union {
  float f;
  Word w;
} FP_Word;


/**************************************************************/


#define EDOM		33


int errno = 0;


/**************************************************************/


void dump(float x) {
  FP_Word X;

  X.f = x;
  printf("%e = 0x%08X = [%c%02X.%06X]",
         X.f, X.w, SGN(X.w) ? '-' : '+',
         EXP(X.w), FRC(X.w));
}


/**************************************************************/


int debug_log = 0;


float fp_log(float x) {
  FP_Word X;

  X.f = x;
  if ((X.w & 0x7FFFFFFF) == 0) {
    errno = EDOM;
    X.w = 0xFF800000;
    return X.f;
  }
  if ((X.w & 0x80000000) != 0) {
    errno = EDOM;
    X.w = 0x7FC00000;
    return X.f;
  }
  /* !!!!! */
  return 0.125;
}


float fp_log10(float x) {
  return fp_log(x) * 0.4342944819;
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


void test_representation(float x) {
  printf("x = ");
  dump(x);
  printf("\n");
  printf("(representation should be 0x3E200000)\n");
}


/**************************************************************/


void test_log_single(float x) {
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


void test_log10_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_log10(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = log10(x);
  printf("r = ");
  dump(r);
  printf("\n");
}


/**************************************************************/


void test_log_many(int count, float lbound, float ubound) {
  int i;
  float x, z, r;
  int below, equal, above, within;
  int ulp;
  FP_Word Z, R;

  printf("operand interval: (%e,%e)\n", lbound, ubound);
  below = 0;
  equal = 0;
  above = 0;
  within = 0;
  ulp = 2;
  for (i = 1; i <= count; i++) {
    if (i % 100 == 0) {
      printf("\rnumber of natural logarithms: %d", i);
      fflush(stdout);
    }
    x = lbound * randl(log(ubound / lbound));
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
    if (ABS(DELTA(Z, R)) <= ulp) {
      within++;
    } else {
      printf("+++++++++++++++++++\n");
      printf("x=");
      dump(x);
      printf("\n");
      debug_log = 1;
      z = fp_log(x);
      debug_log = 0;
      printf("z=");
      dump(z);
      printf("\n");
      printf("r=");
      dump(r);
      printf("\n");
      printf("+++++++++++++++++++\n");
    }
  }
  printf("\n");
  printf("below: %d\n", below);
  printf("equal: %d\n", equal);
  printf("above: %d\n", above);
  printf("within error of %d ulp: %d\n", ulp, within);
}


/**************************************************************/


void test_log10_many(int count, float lbound, float ubound) {
  int i;
  float x, z, r;
  int below, equal, above, within;
  int ulp;
  FP_Word Z, R;

  printf("operand interval: (%e,%e)\n", lbound, ubound);
  below = 0;
  equal = 0;
  above = 0;
  within = 0;
  ulp = 2;
  for (i = 1; i <= count; i++) {
    if (i % 100 == 0) {
      printf("\rnumber of common logarithms: %d", i);
      fflush(stdout);
    }
    x = lbound * randl(log(ubound / lbound));
    z = fp_log10(x);
    r = log10(x);
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
    if (ABS(DELTA(Z, R)) <= ulp) {
      within++;
    } else {
      printf("+++++++++++++++++++\n");
      printf("x=");
      dump(x);
      printf("\n");
      debug_log = 1;
      z = fp_log10(x);
      debug_log = 0;
      printf("z=");
      dump(z);
      printf("\n");
      printf("r=");
      dump(r);
      printf("\n");
      printf("+++++++++++++++++++\n");
    }
  }
  printf("\n");
  printf("below: %d\n", below);
  printf("equal: %d\n", equal);
  printf("above: %d\n", above);
  printf("within error of %d ulp: %d\n", ulp, within);
}


/**************************************************************/


int main(void) {
  printf("------------------------------------------------\n");
  test_representation(0.15625);
  printf("------------------------------------------------\n");
  test_log_single(-1.0);
  printf("------------------------------------------------\n");
  test_log_single(-0.0);
  printf("------------------------------------------------\n");
  test_log_single(0.0);
  printf("------------------------------------------------\n");
  test_log_single(1.0);
  printf("------------------------------------------------\n");
  test_log_single(0.5);
  printf("------------------------------------------------\n");
  test_log_single(2.0);
  printf("------------------------------------------------\n");
#if 0
  test_log_many(10000000, 0.25, 4.0);
  printf("------------------------------------------------\n");
  test_log_many(10000000, 1.0e-15, 1.0);
  printf("------------------------------------------------\n");
  test_log_many(10000000, 1.0, 1.0e15);
  printf("------------------------------------------------\n");
#endif
  printf("------------------------------------------------\n");
  test_log10_single(-1.0);
  printf("------------------------------------------------\n");
  test_log10_single(-0.0);
  printf("------------------------------------------------\n");
  test_log10_single(0.0);
  printf("------------------------------------------------\n");
  test_log10_single(1.0);
  printf("------------------------------------------------\n");
  test_log10_single(0.5);
  printf("------------------------------------------------\n");
  test_log10_single(2.0);
  printf("------------------------------------------------\n");
#if 0
  test_log10_many(10000000, 0.25, 4.0);
  printf("------------------------------------------------\n");
  test_log10_many(10000000, 1.0e-15, 1.0);
  printf("------------------------------------------------\n");
  test_log10_many(10000000, 1.0, 1.0e15);
  printf("------------------------------------------------\n");
#endif
  return 0;
}
