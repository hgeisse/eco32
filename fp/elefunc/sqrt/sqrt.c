/*
 * sqrt.c -- implementation and test of square root
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


int debug_sqrt = 0;


float fp_sqrt(float x) {
  FP_Word X, Z;
  Word expX, expZ;
  Word frcX, frcZ;
  FP_Word A1, A2;

  X.f = x;
  if ((X.w & 0x7FFFFFFF) == 0) {
    return X.f;
  }
  if ((X.w & 0x80000000) != 0) {
    errno = EDOM;
    X.w = 0xFFC00000;
    return X.f;
  }
  expX = EXP(X.w);
  frcX = FRC(X.w);
  A1.w = FLOAT(0, 127, frcX);
  if (debug_sqrt) {
    printf("f=");
    dump(A1.f);
    printf("\n");
  }
  A2.f = 0.59467 + 0.41421 * A1.f;
  if (debug_sqrt) {
    printf("y0=");
    dump(A2.f);
    printf("\n");
  }
  A2.f = 0.5 * (A2.f + A1.f / A2.f);
  if (debug_sqrt) {
    printf("y1=");
    dump(A2.f);
    printf("\n");
  }
  A2.f = 0.5 * (A2.f + A1.f / A2.f);
  if (debug_sqrt) {
    printf("y2=");
    dump(A2.f);
    printf("\n");
  }
  expZ = expX - 127;
  if (expZ & 1) {
    A2.f *= 1.0/sqrt(2.0);
    if (debug_sqrt) {
      printf("corrected y2=");
      dump(A2.f);
      printf("\n");
    }
    expZ++;
  }
  expZ = EXP(A2.w) + expZ / 2;
  frcZ = FRC(A2.w);
  Z.w = FLOAT(0, expZ, frcZ);
  return Z.f;
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


void test_sqrt_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fp_sqrt(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = sqrt(x);
  printf("r = ");
  dump(r);
  printf("\n");
}


/**************************************************************/


void test_sqrt_many(int count, float lbound, float ubound) {
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
      printf("\rnumber of square roots: %d", i);
      fflush(stdout);
    }
    x = lbound * randl(log(ubound / lbound));
    z = fp_sqrt(x);
    r = sqrt(x);
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
      debug_sqrt = 1;
      z = fp_sqrt(x);
      debug_sqrt = 0;
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
  test_sqrt_single(-1.0);
  printf("------------------------------------------------\n");
  test_sqrt_single(-0.0);
  printf("------------------------------------------------\n");
  test_sqrt_single(0.0);
  printf("------------------------------------------------\n");
  test_sqrt_single(1.0);
  printf("------------------------------------------------\n");
  test_sqrt_single(16.0);
  printf("------------------------------------------------\n");
  test_sqrt_single(32.0);
  printf("------------------------------------------------\n");
  test_sqrt_single(1.875);
  printf("------------------------------------------------\n");
  test_sqrt_single(1.234e2);
  printf("------------------------------------------------\n");
  test_sqrt_many(10000000, 0.25, 4.0);
  printf("------------------------------------------------\n");
  test_sqrt_many(10000000, 1.0e-15, 1.0);
  printf("------------------------------------------------\n");
  test_sqrt_many(10000000, 1.0, 1.0e15);
  printf("------------------------------------------------\n");
  return 0;
}
