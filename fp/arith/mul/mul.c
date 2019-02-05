/*
 * mul.c -- implementation and test of floating-point multiplication
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


void dump(float x) {
  FP_Word X;

  X.f = x;
  printf("%e = 0x%08X = (%c,%d,0x%06X)",
         X.f, X.w, SGN(X.w) ? '-' : '+', EXP(X.w), FRC(X.w));
}


/**************************************************************/


static void mul32(Word a, Word b, Word *hp, Word *lp) {
  Word hi, lo;
  Word aux;

  aux = (a >> 16) * (b >> 16);
  hi = aux >> 16;
  lo = aux << 16;
  aux = (a >> 16) * (b & 0xFFFF);
  lo += aux;
  if (lo < aux) {
    hi++;
  }
  aux = (a & 0xFFFF) * (b >> 16);
  lo += aux;
  if (lo < aux) {
    hi++;
  }
  hi = (hi << 16) | (lo >> 16);
  lo = lo << 16;
  aux = (a & 0xFFFF) * (b & 0xFFFF);
  lo += aux;
  if (lo < aux) {
    hi++;
  }
  *hp = hi;
  *lp = lo;
}


static void mul32_ref(Word a, Word b, Word *hp, Word *lp) {
  unsigned long res;

  res = (unsigned long) a * (unsigned long) b;
  *hp = res >> 32;
  *lp = res & 0xFFFFFFFF;
}


void mul32_test(void) {
  int i;
  Word a, b;
  Word hi_ref, lo_ref;
  Word hi, lo;

  printf("--- mul32() test start ---\n");
  for (i = 0; i < 10000000; i++) {
    a = rand();
    b = rand();
    mul32_ref(a, b, &hi_ref, &lo_ref);
    mul32(a, b, &hi, &lo);
    if (hi_ref != hi || lo_ref != lo) {
      printf("%08X * %08X : ref = (%08X,%08X), act = (%08X,%08X)\n",
             a, b, hi_ref, lo_ref, hi, lo);
    }
  }
  printf("--- mul32() test done ---\n");
}


/**************************************************************/


int debug_mul = 0;


float fp_mul(float x, float y) {
  FP_Word X, Y, Z;
  Word sgnX, sgnY, sgnZ;
  Word expX, expY, expZ;
  Word frcX, frcY, frcZ;
  Word hi, lo;
  int round, sticky;

  X.f = x;
  Y.f = y;
  sgnX = SGN(X.w);
  sgnY = SGN(Y.w);
  sgnZ = sgnX ^ sgnY;
  expX = EXP(X.w);
  expY = EXP(Y.w);
  expZ = expX + expY - 127;
  frcX = 0x800000 | FRC(X.w);
  frcY = 0x800000 | FRC(Y.w);
  mul32(frcX, frcY, &hi, &lo);
  if (hi & 0x8000) {
    frcZ = (hi << 8) | (lo >> 24);
    expZ++;
    round = (lo & 0x800000) != 0;
    sticky = (lo & 0x7FFFFF) != 0;
  } else {
    frcZ = (hi << 9) | (lo >> 23);
    round = (lo & 0x400000) != 0;
    sticky = (lo & 0x3FFFFF) != 0;
  }
  if (round && (sticky || (frcZ & 1) != 0)) {
    frcZ++;
    if (frcZ & 0x01000000) {
      frcZ >>= 1;
      expZ++;
    }
  }
  if (debug_mul) {
    printf("frcX = 0x%08X, frcY = 0x%08X, hi = 0x%08X, lo = 0x%08X\n",
           frcX, frcY, hi, lo);
    printf("frcZ = 0x%08X\n", frcZ);
  }
  frcZ &= 0x7FFFFF;
  Z.w = FLOAT(sgnZ, expZ, frcZ);
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


void test_multiply_single(float x, float y) {
  float z;

  printf("x = ");
  dump(x);
  printf("\n");
  printf("y = ");
  dump(y);
  printf("\n");
  z = fp_mul(x, y);
  printf("z = ");
  dump(z);
  printf("\n");
  printf("x*y = %e (ref = %e)\n", z, x * y);
}


/**************************************************************/


void test_multiply_many(int count, float lbound, float ubound) {
  int i;
  float x, y, z, r;
  int below, equal, above, within;
  int ulp;
  FP_Word Z, R;

  printf("operand interval: (%e,%e)\n", lbound, ubound);
  below = 0;
  equal = 0;
  above = 0;
  within = 0;
  ulp = 0;
  for (i = 1; i <= count; i++) {
    if (i % 100 == 0) {
      printf("\rnumber of multiplications: %d", i);
      fflush(stdout);
    }
    x = lbound * randl(log(ubound / lbound));
    y = lbound * randl(log(ubound / lbound));
    z = fp_mul(x, y);
    r = x * y;
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
      printf("y=");
      dump(y);
      printf("\n");
      debug_mul = 1;
      z = fp_mul(x, y);
      debug_mul = 0;
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
  mul32_test();
  printf("------------------------------------------------\n");
  test_representation(0.15625);
  printf("------------------------------------------------\n");
  test_multiply_single(1.0, 1.0);
  printf("------------------------------------------------\n");
  test_multiply_single(2.0, -1.0);
  printf("------------------------------------------------\n");
  test_multiply_single(-1.0, 2.0);
  printf("------------------------------------------------\n");
  test_multiply_single(0.5, 0.5);
  printf("------------------------------------------------\n");
  test_multiply_single(0.75, 0.75);
  printf("------------------------------------------------\n");
  test_multiply_single(-1.234e2, -1.234e-3);
  printf("------------------------------------------------\n");
  test_multiply_many(10000000, 1.0e-15, 1.0);
  printf("------------------------------------------------\n");
  test_multiply_many(10000000, 1.0, 1.0e15);
  printf("------------------------------------------------\n");
  test_multiply_many(10000000, 1.0e-15, 1.0e15);
  printf("------------------------------------------------\n");
  return 0;
}
