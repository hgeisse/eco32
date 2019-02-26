/*
 * add.c -- implementation and test of floating-point addition
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


_FP_Word maxu(_FP_Word A, _FP_Word B) {
  return A > B ? A : B;
}


_FP_Word minu(_FP_Word A, _FP_Word B) {
  return A < B ? A : B;
}


/**************************************************************/


_FP_Word Flags = 0;
int debug = 1;


_FP_Word fp_add(_FP_Word X, _FP_Word Y) {
  _FP_Word absX, absY;
  _FP_Word Sx, Sy, Sr;
  _FP_Word absXm1, absYm1;
  _FP_Word Max;

  if (debug) {
    fprintf(stderr,
            "fp_add(0x%08X [%e], 0x%08X [%e])\n",
            X, *(float *)&X, Y, *(float*)&Y);
  }
  absX = X & 0x7FFFFFFF;
  absY = Y & 0x7FFFFFFF;
  Sx = X & 0x80000000;
  Sy = Y & 0x80000000;
  if (absX > absY) {
    Sr = Sx;
  } else
  if (absX < absY) {
    Sr = Sy;
  } else {
    Sr = minu(Sx, Sy);
  }
  absXm1 = absX - 1;
  absYm1 = absY - 1;
  /* handle special values */
  if (maxu(absXm1, absYm1) >= 0x7F7FFFFF) {
    if (debug) {
      fprintf(stderr,
              "is special\n");
    }
    Max = maxu(absX, absY);
    if (Max > 0x7F800000 ||
        (Sx != Sy && absX == 0x7F800000 && absY == 0x7F800000)) {
      if (((absX > 0x7F800000) && (X & 0x00400000) == 0) ||
          ((absY > 0x7F800000) && (Y & 0x00400000) == 0) ||
          ((absX == 0x7F800000) && (absY == 0x7F800000))) {
        Flags |= _FP_V_FLAG;
      }
      /* return qNaN with payload encoded in last 22 bits */
      return 0x7FC00000 | Max;
    }
    return Sr | Max;
  }
  if (debug) {
    fprintf(stderr,
            "is (sub)normal\n");
  }
  return 0x12345678;
}


float float_add(float x, float y) {
  _FP_Union X, Y, Z;

  X.f = x;
  Y.f = y;
  Z.w = fp_add(X.w, Y.w);
  return Z.f;
}


/**************************************************************/


void test_single(float x, float y) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  printf("y = ");
  dump(y);
  printf("\n");
  z = float_add(x, y);
  printf("z = ");
  dump(z);
  printf("\n");
  r = x + y;
  printf("x*y = %e (ref = %e)\n", z, r);
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
  float x, y, z, r;
  _FP_Union Z, R;

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
    y = lbound * randl(log(ubound / lbound));
    if (maybeNeg) {
      if (rand() & 0x400) {
        y = -y;
      }
    }
    z = float_add(x, y);
    r = x + y;
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
    if (_FP_ABS(_FP_DELTA(Z, R)) <= ulp) {
      within++;
    } else {
      printf("++++++++++++++++++++\n");
      printf("x=");
      dump(x);
      printf("\n");
      printf("y=");
      dump(y);
      printf("\n");
      debug = 1;
      z = float_add(x, y);
      debug = 0;
      printf("z=");
      dump(z);
      printf("\n");
      printf("r=");
      dump(r);
      printf("\n");
      printf("++++++++++++++++++++\n");
    }
  }
  printf("\n");
  printf("below: %d\n", below);
  printf("equal: %d\n", equal);
  printf("above: %d\n", above);
  printf("within error of %d ulp: %d\n", ulp, within);
}


/**************************************************************/


void tests(void) {
  printf("------------------------------------------------\n");
  test_single(1.0, 1.0);
  printf("------------------------------------------------\n");
  test_single(2.0, -1.0);
  printf("------------------------------------------------\n");
  test_single(-1.0, 2.0);
  printf("------------------------------------------------\n");
  test_single(0.5, 0.5);
  printf("------------------------------------------------\n");
  test_single(0.75, 0.75);
  printf("------------------------------------------------\n");
  test_single(-1.234e2, -1.234e-3);
  printf("------------------------------------------------\n");
  test_many(10000000, 0.5, 2.0, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0, 1.0e15, 1);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0e15, 1);
  printf("------------------------------------------------\n");
}


/**************************************************************/


#define LINE_SIZE	200


void server(void) {
  char line[LINE_SIZE];
  char *endptr;
  _FP_Word X, Y, Z;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    X = strtoul(endptr, &endptr, 16);
    Y = strtoul(endptr, &endptr, 16);
    Flags = 0;
    Z = fp_add(X, Y);
    printf("%08X %08X %08X %02X\n", X, Y, Z, Flags);
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
