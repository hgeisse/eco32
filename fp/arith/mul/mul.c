/*
 * mul.c -- implementation and test of floating-point multiplication
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


static void mul32(_FP_Word a, _FP_Word b, _FP_Word *hp, _FP_Word *lp) {
  _FP_Word hi, lo;
  _FP_Word aux;

  aux = (a >> 16) * (b >> 16);
  hi = aux >> 16;
  lo = aux << 16;
  aux = (a >> 16) * (b & 0xFFFF);
  lo += aux;
  if (lo < aux) {
    hi++;
  }
  aux = (a & 0xFFFF) * (b >> 16);
  lo += aux;
  if (lo < aux) {
    hi++;
  }
  hi = (hi << 16) | (lo >> 16);
  lo = lo << 16;
  aux = (a & 0xFFFF) * (b & 0xFFFF);
  lo += aux;
  if (lo < aux) {
    hi++;
  }
  *hp = hi;
  *lp = lo;
}


static void mul32_ref(_FP_Word a, _FP_Word b, _FP_Word *hp, _FP_Word *lp) {
  unsigned long res;

  res = (unsigned long) a * (unsigned long) b;
  *hp = res >> 32;
  *lp = res & 0xFFFFFFFF;
}


void mul32_test(void) {
  int i;
  _FP_Word a, b;
  _FP_Word hi_ref, lo_ref;
  _FP_Word hi, lo;

  printf("--- mul32() test started ---\n");
  for (i = 0; i < 10000000; i++) {
    a = rand();
    b = rand();
    mul32_ref(a, b, &hi_ref, &lo_ref);
    mul32(a, b, &hi, &lo);
    if (hi_ref != hi || lo_ref != lo) {
      printf("%08X * %08X : ref = (%08X,%08X), act = (%08X,%08X)\n",
             a, b, hi_ref, lo_ref, hi, lo);
    }
  }
  printf("--- mul32() test finished ---\n");
}


/**************************************************************/


_FP_Word Flags = 0;
int debug = 0;


_FP_Word fp_mul(_FP_Word X, _FP_Word Y) {
  _FP_Word absX, absY;
  _FP_Word Sr, Inf;
  _FP_Word absXm1, absYm1;
  _FP_Word Min, Max;
  _FP_Word Ex, Ey, nx, ny, MX, MY;
  _FP_Word mpX, mpY, highS, lowS, c, Dm1;
  _FP_Word G, M, highT, lowT, M_OR_lowT, B;
  _FP_Word Z;

  if (debug) {
    fprintf(stderr,
            "fp_mul(0x%08X = %e, 0x%08X = %e)\n",
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
    Min = minu(absX, absY);
    Max = maxu(absX, absY);
    if (Max > 0x7F800000 || (Min == 0 && Max == 0x7F800000)) {
      if (((absX > 0x7F800000) && (X & 0x00400000) == 0) ||
          ((absY > 0x7F800000) && (Y & 0x00400000) == 0) ||
          (Min == 0 && Max == 0x7F800000)) {
        Flags |= _FP_V_FLAG;
      }
      /* return qNaN with payload encoded in last 22 bits */
      return Inf | 0x00400000 | Max;
    }
    if (Max != 0x7F800000) {
      return Sr;
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
  mul32(mpX, mpY, &highS, &lowS);
  c = highS >= 0x80000000;
  Dm1 = (((Ex - nx) + (Ey - ny)) - ((MX + MY) + 110)) + c;
  if (debug) {
    fprintf(stderr,
            "mpX=0x%08X, mpY=%08X, highS=%08X, lowS=%08X, c=%d, Dm1=%d\n",
            mpX, mpY, highS, lowS, c, Dm1);
  }
  if ((int) Dm1 < 0) {
    if (Dm1 < -24) {
      Flags |= _FP_U_FLAG | _FP_X_FLAG;
      return Sr;
    }
    c -= Dm1;
    Dm1 = 0;
  } else
  if (Dm1 >= 0xFE) {
    Flags |= _FP_O_FLAG | _FP_X_FLAG;
    return Inf;
  }
  lowT = (lowS != 0);
  G = (highS >> (6 + c)) & 1;
  M = (c >= 25) ? 0 : (highS >> (7 + c));
  highT = (highS << (26 - c)) != 0;
  M_OR_lowT = M | lowT;
  B = G & (M_OR_lowT | highT);
  if (G | lowT | highT) {
    Flags |= _FP_X_FLAG;
    if (M < 0x00800000) {
      Flags |= _FP_U_FLAG;
    }
  }
  Z = ((Sr | (Dm1 << 23)) + M) + B;
  if (((Z << 1) >> 24) == 0xFF) {
    Flags |= _FP_O_FLAG;
  }
  if (debug) {
    fprintf(stderr,
            "fp_mul = 0x%08X = %e\n",
            Z, *(float *)&Z);
  }
  return Z;
}


float float_mul(float x, float y) {
  _FP_Union X, Y, Z;

  X.f = x;
  Y.f = y;
  Z.w = fp_mul(X.w, Y.w);
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
  z = float_mul(x, y);
  printf("z = ");
  dump(z);
  printf("\n");
  r = x * y;
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
    z = float_mul(x, y);
    r = x * y;
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
      z = float_mul(x, y);
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
  mul32_test();
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
    Z = fp_mul(X, Y);
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
