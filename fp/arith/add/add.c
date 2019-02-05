/*
 * add.c -- implementation and test of floating-point addition
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


int main(void) {
  printf("------------------------------------------------\n");
  test_representation(0.15625);
  printf("------------------------------------------------\n");
  return 0;
}
