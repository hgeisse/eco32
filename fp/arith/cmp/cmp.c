/*
 * cmp.c -- implementation and test of floating-point comparison
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


#define FP_CMP_LT	0		/* less than */
#define FP_CMP_EQ	1		/* equal */
#define FP_CMP_GT	2		/* greater than */
#define FP_CMP_UO	3		/* unordered */


_FP_Word Flags = 0;
int debug = 0;


int fp_cmp(_FP_Word x, _FP_Word y, int invalidIfUnordered) {
  int xIsNaN;
  int yIsNaN;

  if ((x & 0x7FFFFFFF) == 0 && (y & 0x7FFFFFFF) == 0) {
    return FP_CMP_EQ;
  }
  xIsNaN = _FP_EXP(x) == 0xFF && _FP_FRC(x) != 0;
  yIsNaN = _FP_EXP(y) == 0xFF && _FP_FRC(y) != 0;
  if (xIsNaN || yIsNaN) {
    if ((xIsNaN && (_FP_FRC(x) & 0x00400000) == 0) ||
        (yIsNaN && (_FP_FRC(y) & 0x00400000) == 0) ||
        invalidIfUnordered) {
      Flags |= _FP_V_FLAG;
    }
    return FP_CMP_UO;
  }
  if (_FP_SGN(x) != (_FP_SGN(y))) {
    return _FP_SGN(x) ? FP_CMP_LT : FP_CMP_GT;
  }
  if (_FP_SGN(x)) {
    if (x < y) {
      return FP_CMP_GT;
    }
    if (x > y) {
      return FP_CMP_LT;
    }
    return FP_CMP_EQ;
  } else {
    if (x < y) {
      return FP_CMP_LT;
    }
    if (x > y) {
      return FP_CMP_GT;
    }
    return FP_CMP_EQ;
  }
}


int float_cmp(float x, float y, int invalidIfUnordered) {
  _FP_Union X, Y;
  int z;

  X.f = x;
  Y.f = y;
  z = fp_cmp(X.w, Y.w, invalidIfUnordered);
  return z;
}


/**************************************************************/


void test_single(float x, float y) {
  int z;

  printf("x = ");
  dump(x);
  printf("\n");
  printf("y = ");
  dump(y);
  printf("\n");
  z = float_cmp(x, y, 0);
  printf("cmp = ");
  switch (z) {
    case FP_CMP_LT:
      printf("LT");
      break;
    case FP_CMP_EQ:
      printf("EQ");
      break;
    case FP_CMP_GT:
      printf("GT");
      break;
    case FP_CMP_UO:
      printf("UO");
      break;
    default:
      printf("*** unknown ***");
      break;
  }
  printf("\n");
}


/**************************************************************/


void tests(void) {
  printf("------------------------------------------------\n");
  test_single(0.0, -5.0);
  printf("------------------------------------------------\n");
  test_single(-5.0, 0.0);
  printf("------------------------------------------------\n");
  test_single(1.2345, 1.2345);
  printf("------------------------------------------------\n");
  test_single(1.2345, -1.2345);
  printf("------------------------------------------------\n");
  test_single(-1.2345, 1.2345);
  printf("------------------------------------------------\n");
  test_single(-1.2345, -1.2345);
  printf("------------------------------------------------\n");
}


/**************************************************************/


#define LINE_SIZE	200


void server(int cmp) {
  char line[LINE_SIZE];
  char *endptr;
  _FP_Word X, Y;
  int z, r;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    X = strtoul(endptr, &endptr, 16);
    Y = strtoul(endptr, &endptr, 16);
    Flags = 0;
    switch (cmp) {
      case 0:
        /* eq */
        z = fp_cmp(X, Y, 0);
        r = (z == FP_CMP_EQ);
        break;
      case 1:
        /* le */
        z = fp_cmp(X, Y, 1);
        r = (z == FP_CMP_LT || z == FP_CMP_EQ);
        break;
      case 2:
        /* lt */
        z = fp_cmp(X, Y, 1);
        r = (z == FP_CMP_LT);
        break;
    }
    printf("%08X %08X %d %02X\n", X, Y, r, Flags);
  }
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s [-s_eq|-s_le|-s_lt]\n", myself);
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
  if (strcmp(argv[1], "-s_eq") == 0) {
    server(0);
    return 0;
  }
  if (strcmp(argv[1], "-s_le") == 0) {
    server(1);
    return 0;
  }
  if (strcmp(argv[1], "-s_lt") == 0) {
    server(2);
    return 0;
  }
  usage(argv[0]);
  return 1;
}
