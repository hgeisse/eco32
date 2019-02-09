/*
 * abi.c -- show specific details of the ECO32 ABI
 */


#include <stdio.h>


/**************************************************************/


typedef struct {
  char c;
  char a[508];
} Big1;


typedef struct {
  short s;
  char a[507];
} Big2;


typedef struct {
  int i;
  char a[506];
} Big3;


/**************************************************************/


char c;
short s;
int i;
long l;
//long long h;
float f;
double d;
//long double x;
int *p;
Big1 b1;
Big2 b2;
Big3 b3;


/**************************************************************/


/*
 * leaf procedure
 * no args
 * no locals
 */
void g1(void) {
}


/*
 * leaf procedure
 * two args
 * no locals
 */
void g2(int x, int y) {
  x = x ^ y;
  y = x ^ y;
  x = x ^ y;
}


/*
 * leaf procedure
 * no args
 * three locals
 */
void g3(void) {
  int i, j, k;

  i = 1;
  j = 2;
  k = 3;
  i = j + k;
  j = k + i;
  k = i + j;
}


/*
 * leaf procedure
 * two args
 * three locals
 */
void g4(int x, int y) {
  int i, j, k;

  x = x ^ y;
  y = x ^ y;
  x = x ^ y;
  i = 1;
  j = 2;
  k = 3;
  i = j + k;
  j = k + i;
  k = i + j;
}


/* --------------------------- */


/*
 * non-leaf procedure: call with no args, no retval
 * no args
 * no locals
 */
void g5(void) {
  g1();
}


/*
 * non-leaf procedure: call with no args, no retval
 * two args
 * no locals
 */
void g6(int x, int y) {
  g1();
  x = x ^ y;
  y = x ^ y;
  x = x ^ y;
}


/*
 * non-leaf procedure: call with no args, no retval
 * no args
 * three locals
 */
void g7(void) {
  int i, j, k;

  g1();
  i = 1;
  j = 2;
  k = 3;
  i = j + k;
  j = k + i;
  k = i + j;
}


/*
 * non-leaf procedure: call with no args, no retval
 * two args
 * three locals
 */
void g8(int x, int y) {
  int i, j, k;

  g1();
  x = x ^ y;
  y = x ^ y;
  x = x ^ y;
  i = 1;
  j = 2;
  k = 3;
  i = j + k;
  j = k + i;
  k = i + j;
}


#if 0
/*
 * non-leaf procedure: call with no args, no retval
 * no args
 * no locals
 */
void g2(void) {
  g1();
}


/*
 * leaf procedure
 * no args
 * two locals
 */
void g3(void) {
  int i, j;

  i = 2;
  j = 3;
  i = i ^ j;
  j = i ^ j;
  i = i ^ j;
}


/*
 * non-leaf procedure: call with no args, no retval
 * no args
 * two locals
 */
void g4(void) {
  int i, j;

  i = 2;
  j = 3;
  g1();
  i = i ^ j;
  j = i ^ j;
  i = i ^ j;
}
#endif


/**************************************************************/


int main(void) {
  printf("size of char         : %lu\n", sizeof(c));
  printf("size of short        : %lu\n", sizeof(s));
  printf("size of int          : %lu\n", sizeof(i));
  printf("size of long         : %lu\n", sizeof(l));
//  printf("size of long long    : %lu\n", sizeof(h));
  printf("size of float        : %lu\n", sizeof(f));
  printf("size of double       : %lu\n", sizeof(d));
//  printf("size of long double  : %lu\n", sizeof(x));
  printf("size of Type *       : %lu\n", sizeof(p));
  printf("size of struct1      : %lu\n", sizeof(b1));
  printf("size of struct2      : %lu\n", sizeof(b2));
  printf("size of struct3      : %lu\n", sizeof(b3));
  printf("----------------------------------------\n");
  g1();
  g2(11, 22);
  g3();
  g4(11, 22);
  /* --------------------- */
  g5();
  g6(11, 22);
  g7();
  g8(11, 22);
  return 0;
}
