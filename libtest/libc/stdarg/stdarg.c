/*
 * stdarg.c -- test variable argument lists
 */


#include <stdio.h>
#include <stdarg.h>


typedef struct {
  char a[509];
} Big;

Big big;


int f1(int dummy1, char arg, int dummy2, ...) {
  va_list ap1, ap2;

  va_start(ap1, dummy1);
  va_start(ap2, arg);
  va_end(ap1);
  va_end(ap2);
  return (char *) ap2 - (char *) ap1;
}


int f2(int dummy1, short arg, int dummy2, ...) {
  va_list ap1, ap2;

  va_start(ap1, dummy1);
  va_start(ap2, arg);
  va_end(ap1);
  va_end(ap2);
  return (char *) ap2 - (char *) ap1;
}


int f3(int dummy1, int arg, int dummy2, ...) {
  va_list ap1, ap2;

  va_start(ap1, dummy1);
  va_start(ap2, arg);
  va_end(ap1);
  va_end(ap2);
  return (char *) ap2 - (char *) ap1;
}


int f4(int dummy1, long arg, int dummy2, ...) {
  va_list ap1, ap2;

  va_start(ap1, dummy1);
  va_start(ap2, arg);
  va_end(ap1);
  va_end(ap2);
  return (char *) ap2 - (char *) ap1;
}


int f5(int dummy1, float arg, int dummy2, ...) {
  va_list ap1, ap2;

  va_start(ap1, dummy1);
  va_start(ap2, arg);
  va_end(ap1);
  va_end(ap2);
  return (char *) ap2 - (char *) ap1;
}


int f6(int dummy1, double arg, int dummy2, ...) {
  va_list ap1, ap2;

  va_start(ap1, dummy1);
  va_start(ap2, arg);
  va_end(ap1);
  va_end(ap2);
  return (char *) ap2 - (char *) ap1;
}


int f7(int dummy1, int *arg, int dummy2, ...) {
  va_list ap1, ap2;

  va_start(ap1, dummy1);
  va_start(ap2, arg);
  va_end(ap1);
  va_end(ap2);
  return (char *) ap2 - (char *) ap1;
}


int f8(int dummy1, Big arg, int dummy2, ...) {
  va_list ap1, ap2;

  va_start(ap1, dummy1);
  va_start(ap2, arg);
  va_end(ap1);
  va_end(ap2);
  return (char *) ap2 - (char *) ap1;
}


int main(void) {
  int delta;

  delta = f1(0, 'a', 42, 7, 8, 9);
  printf("stack size of char   = %d\n", delta);
  delta = f2(0, (short) 12, 7, 8, 9);
  printf("stack size of short  = %d\n", delta);
  delta = f3(0, (int) 12, 7, 8, 9);
  printf("stack size of int    = %d\n", delta);
  delta = f4(0, (long) 12, 7, 8, 9);
  printf("stack size of long   = %d\n", delta);
  delta = f5(0, (float) 123.456, 7, 8, 9);
  printf("stack size of float  = %d\n", delta);
  delta = f6(0, (double) 123.456, 7, 8, 9);
  printf("stack size of double = %d\n", delta);
  delta = f7(0, &delta, 7, 8, 9);
  printf("stack size of int*   = %d\n", delta);
  delta = f8(0, big, 7, 8, 9);
  printf("stack size of struct = %d\n", delta);
  return 0;
}
