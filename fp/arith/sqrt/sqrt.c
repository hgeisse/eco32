/*
 * sqrt.c -- implementation and test of floating-point square root
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


_FP_Word fp_sqrt(_FP_Word X) {
  _FP_Word Xm1;
  _FP_Word Ex, nx, MX, Dm1;
  _FP_Word mpX, c;
  _FP_Word Q, R, S;
  int i, T;
  _FP_Word Z;

  if (debug) {
    fprintf(stderr,
            "fp_sqrt(0x%08X = %e)\n",
            X, *(float *)&X);
  }
  Xm1 = X - 1;
  /* handle special values */
  if (Xm1 >= 0x7F7FFFFF) {
    if (debug) {
      fprintf(stderr,
              "is special\n");
    }
    if (X <= 0x7F800000 || X == 0x80000000) {
      return X;
    }
    if ((X & 0x00400000) == 0 ||
        (X > 0x80000000 && X < 0xFF800000)) {
      Flags |= _FP_V_FLAG;
    }
    /* return qNaN with payload encoded in last 22 bits */
    return 0x7FC00000 | X;
  }
  if (debug) {
    fprintf(stderr,
            "is (sub)normal\n");
  }
  Ex = X >> 23;
  nx = X >= 0x00800000;
  MX = maxu(nlz32(X), 8);
  Dm1 = ((Ex - nx) + (134 - MX)) >> 1;
  mpX = (X << MX) | 0x80000000;
  c = ((Ex - nx) - MX) & 1;
  if (debug) {
    fprintf(stderr,
            "Ex=%d, nx=%d, MX=%d, Dm1=%d, mpX=0x%08X, c=%d\n",
            Ex, nx, MX, Dm1, mpX, c);
  }
  Q = 0x01000000;
  R = (mpX >> (7 - c)) - Q;
  S = 0x01000000;
  for (i = 1; i < 25; i++) {
    S >>= 1;
    T = (R << 1) - ((Q << 1) + S);
    if (T < 0) {
      R <<= 1;
    } else {
      Q += S;
      R = T;
    }
  }
  if (R != 0) {
    Flags |= _FP_X_FLAG;
  }
  Z = (Dm1 << 23) + ((Q >> 1) + (Q & 1));
  if (debug) {
    fprintf(stderr,
            "fp_sqrt = 0x%08X = %e\n",
            Z, *(float *)&Z);
  }
  return Z;
}


float float_sqrt(float x) {
  _FP_Union X, Z;

  X.f = x;
  Z.w = fp_sqrt(X.w);
  return Z.f;
}


/**************************************************************/


void test_single(float x) {
  float z, r;

  printf("x = ");
  dump(x);
  printf("\n");
  z = float_sqrt(x);
  printf("z = ");
  dump(z);
  printf("\n");
  r = sqrt(x);
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
  float x, z, r;
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
    z = float_sqrt(x);
    r = sqrt(x);
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
      debug = 1;
      z = float_sqrt(x);
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
  test_single(1.0);
  printf("------------------------------------------------\n");
  test_single(2.0);
  printf("------------------------------------------------\n");
  test_single(0.5);
  printf("------------------------------------------------\n");
  test_single(1.234e2);
  printf("------------------------------------------------\n");
  test_single(1.234e-3);
  printf("------------------------------------------------\n");
  test_many(10000000, 0.5, 2.0, 0);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0, 0);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0, 1.0e15, 0);
  printf("------------------------------------------------\n");
  test_many(10000000, 1.0e-15, 1.0e15, 0);
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
    Z = fp_sqrt(X);
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
