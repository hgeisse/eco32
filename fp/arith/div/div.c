/*
 * div.c -- implementation and test of floating-point division
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


/**************************************************************/


static _FP_Word nlz32(_FP_Word X) {
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


static _FP_Word nlz32_ref(_FP_Word X) {
  int n;

  n = 0;
  while ((n < 32) && (X & 0x80000000) == 0) {
    n++;
    X <<= 1;
  }
  return n;
}


void nlz32_test(void) {
  int i;
  _FP_Word X;
  int k;
  _FP_Word Mask;
  int n_ref;
  int n;

  printf("--- nlz32() test started ---\n");
  for (i = 0; i < 1000000; i++) {
    X = (random() << 4) ^ random();
    k = random() % 33;
    if (k == 0) {
      /* 0 leading bits zero, i.e., keep X as is */
      Mask = 0xFFFFFFFF;
    } else
    if (k == 32) {
      /* 32 leading bits zero, i.e., all bits zero */
      Mask = 0x00000000;
    } else {
      /* at least k leading bits zero */
      Mask = ~(((1 << k) - 1) << (32 - k));
    }
    X &= Mask;
    n_ref = nlz32_ref(X);
    n = nlz32(X);
    if (n_ref != n) {
      printf("nlz32(%08X) : ref = %2d, act = %2d\n",
             X, n_ref, n);
    }
  }
  printf("--- nlz32() test finished ---\n");
}


/**************************************************************/


_FP_Word Flags = 0;
int debug = 0;


_FP_Word fp_div(_FP_Word X, _FP_Word Y) {
  _FP_Word absX, absY;
  _FP_Word Sr, Inf;
  _FP_Word absXm1, absYm1;
  _FP_Word Max;
  _FP_Word Ex, Ey, nx, ny, MX, MY;
  _FP_Word mpX, mpY, c, Dm1;
  _FP_Word N, M, Q, R;
  int i, T;
  _FP_Word Z;

  if (debug) {
    fprintf(stderr,
            "fp_div(0x%08X = %e, 0x%08X = %e)\n",
            X, *(float *)&X, Y, *(float *)&Y);
  }
  absX = X & 0x7FFFFFFF;
  absY = Y & 0x7FFFFFFF;
  Sr = (X ^ Y) & 0x80000000;
  Inf = Sr | 0x7F800000;
  absXm1 = absX - 1;
  absYm1 = absY - 1;
  /* handle special values */
  if (maxu(absXm1, absYm1) >= 0x7F7FFFFF) {
    if (debug) {
      fprintf(stderr,
              "is special\n");
    }
    Max = maxu(absX, absY);
    if (Max > 0x7F800000 || absX == absY) {
      if (((absX > 0x7F800000) && (X & 0x00400000) == 0) ||
          ((absY > 0x7F800000) && (Y & 0x00400000) == 0) ||
          (absX == 0x7F800000 && absY == 0x7F800000) ||
          (absX == 0 && absY == 0)) {
        Flags |= _FP_V_FLAG;
      }
      /* return qNaN with payload encoded in last 22 bits */
      return Inf | 0x00400000 | Max;
    }
    if (absX < absY) {
      return Sr;
    }
    if (absX != 0x7F800000 && absY == 0) {
      Flags |= _FP_I_FLAG;
    }
    return Inf;
  }
  if (debug) {
    fprintf(stderr,
            "is (sub)normal\n");
  }
  Ex = absX >> 23;
  Ey = absY >> 23;
  nx = absX >= 0x00800000;
  ny = absY >= 0x00800000;
  MX = maxu(nlz32(absX), 8);
  MY = maxu(nlz32(absY), 8);
  if (debug) {
    fprintf(stderr,
            "Ex=%d, Ey=%d, nx=%d, ny=%d, MX=%d, MY=%d\n",
            Ex, Ey, nx, ny, MX, MY);
  }
  mpX = (X << MX) | 0x80000000;
  mpY = (Y << MY) | 0x80000000;
  c = mpX >= mpY;
  Dm1 = (((Ex - nx) - (Ey - ny)) - ((MX - MY) - 125)) + c;
  if (debug) {
    fprintf(stderr,
            "mpX=0x%08X, mpY=%08X, c=%d, Dm1=%d\n",
            mpX, mpY, c, Dm1);
  }
  if ((int) Dm1 < 0) {
    Dm1 = 0;
  } else
  if (Dm1 >= 0xFE) {
    Flags |= _FP_O_FLAG | _FP_X_FLAG;
    return Inf;
  }
  N = mpX >> (7 + c);
  M = mpY >> 8;
  Q = 1;
  R = N - M;
  for (i = 1; i < 25; i++) {
    T = (R << 1) - M;
    if (T < 0) {
      Q = Q << 1;
      R = T + M;
    } else {
      Q = (Q << 1) + 1;
      R = T;
    }
  }
  if (R != 0) {
    Flags |= _FP_X_FLAG;
  }
  Z = (Sr | (Dm1 << 23)) + ((Q >> 1) + (Q & 1));
  if (debug) {
    fprintf(stderr,
            "fp_div = 0x%08X = %e\n",
            Z, *(float *)&Z);
  }
  return Z;
}


float float_div(float x, float y) {
  _FP_Union X, Y, Z;

  X.f = x;
  Y.f = y;
  Z.w = fp_div(X.w, Y.w);
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
  z = float_div(x, y);
  printf("z = ");
  dump(z);
  printf("\n");
  r = x / y;
  printf("r = ");
  dump(r);
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
    z = float_div(x, y);
    r = x / y;
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
      z = float_div(x, y);
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
  nlz32_test();
  printf("------------------------------------------------\n");
  //mul32_test();
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
    Z = fp_div(X, Y);
    printf("%08X %08X %08X %02X\n", X, Y, Z, Flags);
  }
}


/**************************************************************/


void ttt(void) {
  _FP_Word X, Y, Z;

  X = 0x3F800000;
  Y = 0x3F800000;
  Z = fp_div(X, Y);
  printf("0x%08X / 0x%08X = 0x%08X\n", X, Y, Z);
  exit(1);
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s [-s]\n", myself);
}


int main(int argc, char *argv[]) {
  //ttt();
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
