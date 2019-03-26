/*
 * cf2i.c -- implementation and test of float-to-integer conversion
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


/**************************************************************/


void dump(float x) {
  _FP_Union X;

  X.f = x;
  printf("%e = 0x%08X = [%c%02X.%06X]",
         X.f, X.w, _FP_SGN(X.w) ? '-' : '+',
         _FP_EXP(X.w), _FP_FRC(X.w));
}


/**************************************************************/


_FP_Word Flags = 0;
int debug = 0;


_FP_Word fp_cf2i(_FP_Word x) {
  int expX;
  _FP_Word frcX;
  _FP_Word z;

  expX = (int) _FP_EXP(x) - 127;
  if (expX < 0) {
    return 0x00000000;
  }
  if (debug) {
    printf("expX = %d\n", expX);
  }
  frcX = _FP_FRC(x) | 0x00800000;
  if (expX <= 23) {
    z = frcX >> (23 - expX);
  } else
  if (expX <= 30) {
    z = frcX << (expX - 23);
  } else {
    if (x != 0xCF000000) {
      Flags |= _FP_V_FLAG;
    }
    return 0x80000000;
  }
  return _FP_SGN(x) ? -z : z;
}


int float_cf2i(float x) {
  _FP_Union X;
  int z;

  X.f = x;
  z = (int) fp_cf2i(X.w);
  return z;
}


/**************************************************************/


void test_single(float x) {
  int z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = float_cf2i(x);
  printf("z = 0x%08X\n", z);
  r = (int) x;
  printf("r = 0x%08X\n", r);
}


/**************************************************************/


void tests(void) {
  printf("------------------------------------------------\n");
  test_single(0.0);
  printf("------------------------------------------------\n");
  test_single(-0.0);
  printf("------------------------------------------------\n");
  test_single(2.3);
  printf("------------------------------------------------\n");
  test_single(-2.3);
  printf("------------------------------------------------\n");
  test_single(2.5);
  printf("------------------------------------------------\n");
  test_single(-2.5);
  printf("------------------------------------------------\n");
  test_single(2.7);
  printf("------------------------------------------------\n");
  test_single(-2.7);
  printf("------------------------------------------------\n");
  test_single(3.0);
  printf("------------------------------------------------\n");
  test_single(-3.0);
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


#define LINE_SIZE	200


void server(void) {
  char line[LINE_SIZE];
  char *endptr;
  _FP_Word X, Z;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    X = strtoul(endptr, &endptr, 16);
    Flags = 0;
    Z = fp_cf2i(X);
    printf("%08X %08X %02X\n", X, Z, Flags);
  }
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s [-s]\n", myself);
}


int main(int argc, char *argv[]) {
  if (argc == 1) {
    tests();
    return 0;
  }
  if (argc != 2) {
    usage(argv[0]);
    return 1;
  }
  if (strcmp(argv[1], "-s") == 0) {
    server();
    return 0;
  }
  usage(argv[0]);
  return 1;
}
