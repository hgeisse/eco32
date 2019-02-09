/*
 * fabs.c -- test absolute value
 */


#include <stdio.h>
#include <math.h>
#include <sys/fp.h>


/**************************************************************/


void dump(float x) {
  _FP_Union X;

  X.f = x;
  printf("%e = 0x%08X = (%c,%d,0x%06X)",
         X.f, X.w, _FP_SGN(X.w) ? '-' : '+',
         _FP_EXP(X.w), _FP_FRC(X.w));
}


/**************************************************************/


void test_single(float x) {
  float z;

  printf("x = ");
  dump(x);
  printf("\n");
  z = fabs(x);
  printf("z = ");
  dump(z);
  printf("\n");
//  r = gnu_fabs(x);
//  printf("r = ");
//  dump(r);
//  printf("\n");
}


/**************************************************************/


void test_many(int count, float lbound, float ubound) {
}


/**************************************************************/


int main(void) {
  printf("------------------------------------------------\n");
  test_single(-1.0);
  printf("------------------------------------------------\n");
  test_single(-0.0);
  printf("------------------------------------------------\n");
  test_single(+0.0);
  printf("------------------------------------------------\n");
  test_single(+1.0);
  printf("------------------------------------------------\n");
  test_single(-123.456);
  printf("------------------------------------------------\n");
  test_single(+123.456);
  printf("------------------------------------------------\n");
  return 0;
}
