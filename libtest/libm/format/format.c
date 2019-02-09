/*
 * format.c -- test the representation of fp numbers
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../lib/include/sys/fp.h"


/**************************************************************/


void dump(float x) {
  _FP_Union X;

  X.f = x;
  printf("%e = 0x%08X = (%c,%d,0x%06X)",
         X.f, X.w, _FP_SGN(X.w) ? '-' : '+',
         _FP_EXP(X.w), _FP_FRC(X.w));
}


/**************************************************************/


void test_single(float x, _FP_Word w) {
  _FP_Union R;

  printf("x = ");
  dump(x);
  printf("\n");
  R.w = w;
  printf("r = ");
  dump(R.f);
  printf("\n");
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
  float x, z;
  _FP_Union X, Z;
  _FP_Word sgn, exp, frc;

  printf("absolute operand interval: (%e,%e)\n", lbound, ubound);
  printf("operands may%sbe negative\n", maybeNeg ? " " : " not ");
  below = 0;
  equal = 0;
  above = 0;
  within = 0;
  ulp = 1;
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
    X.f = x;
    sgn = _FP_SGN(X.w);
    exp = _FP_EXP(X.w);
    frc = _FP_FRC(X.w);
    Z.w = _FP_FLT(sgn, exp, frc);
    z = Z.f;
    if (z < x) {
      below++;
    } else
    if (z > x) {
      above++;
    } else {
      equal++;
    }
    if (_FP_ABS(_FP_DELTA(Z, X)) <= ulp) {
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


int main(void) {
  printf("------------------------------------------------\n");
  test_single(-1.0, 0xBF800000);
  printf("------------------------------------------------\n");
  test_single(-0.0, 0x80000000);
  printf("------------------------------------------------\n");
  test_single(+0.0, 0x00000000);
  printf("------------------------------------------------\n");
  test_single(+1.0, 0x3F800000);
  printf("------------------------------------------------\n");
  test_single(-123.456, 0xC2F6E979);
  printf("------------------------------------------------\n");
  test_single(+123.456, 0x42F6E979);
  printf("------------------------------------------------\n");
  test_many(100000, 1.0e-25, 1.0e25, 1);
  printf("------------------------------------------------\n");
  return 0;
}
