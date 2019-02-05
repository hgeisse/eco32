/*
 * div.c -- implementation and test of floating-point division
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define SGN(x)		((x) & 0x80000000)
#define EXP(x)		(((x) << 1) >> 24)
#define FRC(x)		((x) & 0x7FFFFF)
#define FLOAT(s,e,f)	(s | (e << 23) | f)


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


static void div32(Word a, Word b, Word *hp, Word *lp) {
  unsigned long res;

  res = ((unsigned long) a << 32) / (unsigned long) b;
  *hp = res >> 32;
  *lp = res & 0xFFFFFFFF;
}


static void div32_ref(Word a, Word b, Word *hp, Word *lp) {
  unsigned long res;

  res = ((unsigned long) a << 32) / (unsigned long) b;
  *hp = res >> 32;
  *lp = res & 0xFFFFFFFF;
}


void div32_test(void) {
  int i;
  Word a, b;
  Word hi_ref, lo_ref;
  Word hi, lo;

  printf("--- div32() test start ---\n");
  for (i = 0; i < 10000000; i++) {
    a = rand();
    b = rand();
    div32_ref(a, b, &hi_ref, &lo_ref);
    div32(a, b, &hi, &lo);
    if (hi_ref != hi || lo_ref != lo) {
      printf("%08X / %08X : ref = (%08X,%08X), act = (%08X,%08X)\n",
             a, b, hi_ref, lo_ref, hi, lo);
    }
  }
  printf("--- div32() test done ---\n");
}


/**************************************************************/


float fp_div(float x, float y) {
  FP_Word X, Y, Z;
  Word sgnX, sgnY, sgnZ;
  Word expX, expY, expZ;
  Word frcX, frcY;

  X.f = x;
  Y.f = y;
  sgnX = SGN(X.w);
  sgnY = SGN(Y.w);
  sgnZ = sgnX ^ sgnY;
  expX = EXP(X.w);
  expY = EXP(Y.w);
  expZ = expX - expY + 127;
  frcX = 0x800000 | FRC(X.w);
  frcY = 0x800000 | FRC(Y.w);
  printf("frcX = 0x%08X, frcY = 0x%08X\n", frcX, frcY);
  /*---*/
  Z.w = FLOAT(sgnZ, expZ, 0);
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


void test_divide_single(float x, float y) {
  float z;

  printf("x = ");
  dump(x);
  printf("\n");
  printf("y = ");
  dump(y);
  printf("\n");
  z = fp_div(x, y);
  printf("z = ");
  dump(z);
  printf("\n");
  printf("x/y = %e (ref = %e)\n", z, x / y);
}


/**************************************************************/


void test_divide_many(int count, float lbound, float ubound) {
  int i;
  float x, y, z, r;
  int below, equal, above, within;
  float eps;

  printf("operand interval: (%e,%e)\n", lbound, ubound);
  below = 0;
  equal = 0;
  above = 0;
  within = 0;
  eps = 1.2e-7;
  for (i = 1; i <= count; i++) {
    if (i % 100 == 0) {
      printf("\rnumber of divisions: %d", i);
      fflush(stdout);
    }
    x = lbound * randl(log(ubound / lbound));
    y = lbound * randl(log(ubound / lbound));
    z = fp_div(x, y);
    r = x / y;
    if (z < r) {
      below++;
    } else
    if (z > r) {
      above++;
    } else {
      equal++;
    }
    if (fabs(z - r) / fabs(r) < eps) {
      within++;
    }
  }
  printf("\n");
  printf("below: %d\n", below);
  printf("equal: %d\n", equal);
  printf("above: %d\n", above);
  printf("within relative error of %e: %d\n", eps, within);
}


/**************************************************************/


int main(void) {
  printf("------------------------------------------------\n");
  div32_test();
  printf("------------------------------------------------\n");
  test_representation(0.15625);
  printf("------------------------------------------------\n");
  test_divide_single(1.0, 1.0);
  printf("------------------------------------------------\n");
  test_divide_single(8.0, 2.0);
  printf("------------------------------------------------\n");
  test_divide_single(1.001e3, 1300.0e-2);
  printf("------------------------------------------------\n");
  return 0;
}
