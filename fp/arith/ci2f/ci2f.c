/*
 * ci2f.c -- implementation and test of integer-to-float conversion
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


_FP_Word nlz32(_FP_Word X) {
  _FP_Word Z;

  Z = 0;
  if (X == 0) {
    return 32;
  }
  if (X <= 0x0000FFFF) {
    Z += 16;
    X <<= 16;
  }
  if (X <= 0x00FFFFFF) {
    Z += 8;
    X <<= 8;
  }
  if (X <= 0x0FFFFFFF) {
    Z += 4;
    X <<= 4;
  }
  if (X <= 0x3FFFFFFF) {
    Z += 2;
    X <<= 2;
  }
  if (X <= 0x7FFFFFFF) {
    Z += 1;
  }
  return Z;
}


/**************************************************************/


_FP_Word Flags = 0;
int debug = 0;


_FP_Word fp_ci2f(_FP_Word x) {
  _FP_Word signX;
  int n;
  _FP_Word frcZ;
  int r, s;
  _FP_Word z;

  if (x == 0x00000000) {
    return 0x00000000;
  }
  signX = x & 0x80000000;
  if (signX) {
    x = -x;
  }
  n = nlz32(x);
  if (debug) {
    printf("n = %d\n", n);
  }
  if (n >= 8) {
    frcZ = x << (n - 8);
    r = 0;
    s = 0;
  } else {
    frcZ = x >> (8 - n);
    r = (x & (0x00000001 << (7 - n))) != 0;
    s = (x & ((0x00000001 << (7 - n)) - 1)) != 0;
  }
  if (debug) {
    printf("frcZ = 0x%08X, r = %d, s = %d\n", frcZ, r, s);
  }
  if (r | s) {
    Flags |= _FP_X_FLAG;
  }
  z = _FP_FLT(signX, 158 - n, frcZ & 0x007FFFFF) + (r & (frcZ | s));
  return z;
}


float float_ci2f(int x) {
  _FP_Union Z;
  float z;

  Z.w = fp_ci2f((_FP_Word) x);
  z = Z.f;
  return z;
}


/**************************************************************/


void test_single(int x) {
  float z, r;

  printf("x = 0x%08X", x);
  printf("\n");
  z = float_ci2f(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = (float) x;
  printf("r = ");
  dump(r);
  printf("\n");
}


/**************************************************************/


void tests(void) {
  printf("------------------------------------------------\n");
  test_single(0);
  printf("------------------------------------------------\n");
  test_single(-0);
  printf("------------------------------------------------\n");
  test_single(2);
  printf("------------------------------------------------\n");
  test_single(-2);
  printf("------------------------------------------------\n");
  test_single(127);
  printf("------------------------------------------------\n");
  test_single(-127);
  printf("------------------------------------------------\n");
  test_single(0x12345678);
  printf("------------------------------------------------\n");
  test_single(-0x12345678);
  printf("------------------------------------------------\n");
  test_single(0x003FFFFF);
  printf("------------------------------------------------\n");
  test_single(0x00400000);
  printf("------------------------------------------------\n");
  test_single(0x007FFFFF);
  printf("------------------------------------------------\n");
  test_single(0x00800000);
  printf("------------------------------------------------\n");
  test_single(0x00FFFFFF);
  printf("------------------------------------------------\n");
  test_single(0x01000000);
  printf("------------------------------------------------\n");
  test_single(0x01FFFFFF);
  printf("------------------------------------------------\n");
  test_single(0x02000000);
  printf("------------------------------------------------\n");
  test_single(0x01000001);
  printf("------------------------------------------------\n");
  test_single(0x01000002);
  printf("------------------------------------------------\n");
  test_single(0x01000003);
  printf("------------------------------------------------\n");
  test_single(0x3E7F7F7F);
  printf("------------------------------------------------\n");
  test_single(-0x3E7F7F7F);
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
    Z = fp_ci2f(X);
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
