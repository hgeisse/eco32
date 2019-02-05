/*
 * basic.c -- basic floating point routines
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/**************************************************************/


typedef unsigned int Word;


#define SIGN		(1 << (sizeof(Word) * 8 - 1))

#define maxu(x, y)	((Word)(x) > (Word)(y) ? (Word)(x) : (Word)(y))
#define minu(x, y)	((Word)(x) < (Word)(y) ? (Word)(x) : (Word)(y))
#define max(x, y)	((int)(x) > (int)(y) ? (int)(x) : (int)(y))
#define min(x, y)	((int)(x) < (int)(y) ? (int)(x) : (int)(y))

#define V_FLAG		0x10
#define I_FLAG		0x08
#define O_FLAG		0x04
#define U_FLAG		0x02
#define X_FLAG		0x01


/**************************************************************/


static void dumpFloat(Word X) {
  Word e;
  Word m;

  if (X & 0x80000000) {
    printf("-");
  } else {
    printf("+");
  }
  e = (X & 0x7FFFFFFF) >> 23;
  printf("%02X.", e);
  m = X & 0x007FFFFF;
  printf("%06X", m);
}


static void dumpFlags(Word Flags) {
  printf("%c", (Flags & 0x10) ? 'v' : '.');
  printf("%c", (Flags & 0x08) ? 'i' : '.');
  printf("%c", (Flags & 0x04) ? 'o' : '.');
  printf("%c", (Flags & 0x02) ? 'u' : '.');
  printf("%c", (Flags & 0x01) ? 'x' : '.');
}


void dumpOpsAndRes(Word X, Word Y, Word Z, Word Flags) {
  dumpFloat(X);
  printf("  ");
  dumpFloat(Y);
  printf("  => ");
  dumpFloat(Z);
  printf(" ");
  dumpFlags(Flags);
  printf("\n");
}


/**************************************************************/


static Word nlz32(Word X) {
  Word Z;

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


static Word nlz32_ref(Word X) {
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
  Word X;
  int k;
  int n_ref;
  int n;

  printf("--- nlz32() test ---\n");
  for (i = 0; i < 1000000; i++) {
    X = (random() << 2) | random();
    k = random() % 33;
    if (k == 0) {
      /* 0 MSBs zero, keep X as is */
    } else
    if (k == 32) {
      /* 32 MSBs zero, i.e., all bits */
      X = 0;
    } else {
      /* k MSBs zero */
      X &= ~(((1 << k) - 1) << (32 - k));
    }
    n_ref = nlz32_ref(X);
    n = nlz32(X);
    if (n_ref != n) {
      printf("nlz32(%08X) : ref = %2d, act = %2d\n",
             X, n_ref, n);
    }
  }
  printf("--- nlz32() test done ---\n");
}


/**************************************************************/


static void mul32(Word a, Word b, Word *hp, Word *lp) {
  Word Hi, Lo;
  Word Aux;

  Aux = (a >> 16) * (b >> 16);
  Hi = Aux >> 16;
  Lo = Aux << 16;
  Aux = (a >> 16) * (b & 0xFFFF);
  Lo += Aux;
  if (Lo < Aux) {
    Hi++;
  }
  Aux = (a & 0xFFFF) * (b >> 16);
  Lo += Aux;
  if (Lo < Aux) {
    Hi++;
  }
  Hi = (Hi << 16) | (Lo >> 16);
  Lo = Lo << 16;
  Aux = (a & 0xFFFF) * (b & 0xFFFF);
  Lo += Aux;
  if (Lo < Aux) {
    Hi++;
  }
  *hp = Hi;
  *lp = Lo;
}


static void mul32_ref(Word n, Word m, Word *hi, Word *lo) {
  unsigned long res;

  res = (unsigned long) n * (unsigned long) m;
  *hi = res >> 32;
  *lo = res & 0xFFFFFFFF;
}


void mul32_test(void) {
  int i;
  Word X, Y;
  Word Hi_ref, Lo_ref;
  Word Hi, Lo;

  printf("--- mul32() test ---\n");
  for (i = 0; i < 10000000; i++) {
    X = random();
    Y = random();
    mul32_ref(X, Y, &Hi_ref, &Lo_ref);
    mul32(X, Y, &Hi, &Lo);
    if (Hi_ref != Hi || Lo_ref != Lo) {
      printf("%08X * %08X : ref = (%08X,%08X), act = (%08X,%08X)\n",
             X, Y, Hi_ref, Lo_ref, Hi, Lo);
    }
  }
  printf("--- mul32() test done ---\n");
}


/**************************************************************/


Word Flags;


#define int_fadd	int_fadd_1
#define int_fsub	int_fsub_1
#define int_fmul	int_fmul_1a
#define int_fdiv	int_fdiv_1
#define int_fsqrt	int_fsqrt_1


/**************************************************************/


Word int_fadd_1(Word X, Word Y) {
  return 0;
}


Word int_fsub_1(Word X, Word Y) {
  return 0;
}


Word int_fmul_1a(Word X, Word Y) {
  Word absX, absY;
  Word Sr, Inf;
  Word absXm1, absYm1;
  Word Min, Max;
  Word Ex, Ey, nx, ny, MX, MY;
  int Dm1;
  Word rshift;
  Word mpX, mpY, highS, lowS, c;
  Word G, M, highT, lowT, M_OR_lowT, B;

  absX = X & 0x7FFFFFFF;
  absY = Y & 0x7FFFFFFF;
  Sr = (X ^ Y) & 0x80000000;
  Inf = Sr | 0x7F800000;
  absXm1 = absX - 1;
  absYm1 = absY - 1;
  /* handle special values */
  if (maxu(absXm1, absYm1) >= 0x7F7FFFFF) {
    Min = minu(absX, absY);
    Max = maxu(absX, absY);
    if (Max > 0x7F800000 || (Min == 0 && Max == 0x7F800000)) {
      if (((absX > 0x7F800000) && (X & 0x00400000) == 0) ||
          ((absY > 0x7F800000) && (Y & 0x00400000) == 0) ||
          (Min == 0 && Max == 0x7F800000)) {
        Flags |= V_FLAG;
      }
      /* return qNaN with payload encoded in last 22 bits */
      return Inf | 0x00400000 | Max;
    }
    if (Max != 0x7F800000) {
      return Sr;
    }
    return Inf;
  }
  Ex = absX >> 23;
  Ey = absY >> 23;
  nx = absX >= 0x00800000;
  ny = absY >= 0x00800000;
  MX = maxu(nlz32(absX), 8);
  MY = maxu(nlz32(absY), 8);
  mpX = (X << MX) | 0x80000000;
  mpY = (Y << MY) | 0x80000000;
  mul32(mpX, mpY, &highS, &lowS);
  c = highS >= 0x80000000;
  Dm1 = (((Ex - nx) + (Ey - ny)) - ((MX + MY) + 110)) + c;
  if (Dm1 < 0) {
    rshift = minu(-Dm1, 32);
    if (rshift == 32) {
      if (lowS != 0 || highS != 0) {
        Flags |= X_FLAG;
      }
      lowS = 0;
      highS = 0;
    } else {
      if (lowS & ((1 << rshift) - 1)) {
        Flags |= X_FLAG;
      }
      lowS = ((highS >> (32 - rshift)) << (32 - rshift)) | (lowS >> rshift);
      highS = highS >> rshift;
    }
    Dm1 = 0;
    Flags |= U_FLAG;
    return 0x12345678;
  }
  if (Dm1 >= 0xFE) {
    Flags |= O_FLAG | X_FLAG;
    return Inf;
  }
  lowT = (lowS != 0);
  G = (highS >> (6 + c)) & 1;
  M = highS >> (7 + c);
  highT = (highS << (26 - c)) != 0;
  M_OR_lowT = M | lowT;
  B = G & (M_OR_lowT | highT);
  if (G | highT | lowT) {
    Flags |= X_FLAG;
  }
  return ((Sr | (Dm1 << 23)) + M) + B;
}


Word int_fmul_1b(Word X, Word Y) {
  Word absX, absY;
  Word Sr, Inf;
  Word absXm1, absYm1;
  Word Min, Max;
  Word Ex, Ey, nx, ny, MX, MY, Dm1;
  Word mpX, mpY, highS, lowS, c;
  Word G, M, highT, lowT, M_OR_lowT, B;
  Word Res;

  printf("fmul(0x%08X [%e], 0x%08X [%e])\n",
               X, *(float *)&X, Y, *(float*)&Y);
  absX = X & 0x7FFFFFFF;
  absY = Y & 0x7FFFFFFF;
  Sr = (X ^ Y) & 0x80000000;
  Inf = Sr | 0x7F800000;
  absXm1 = absX - 1;
  absYm1 = absY - 1;
  /* handle special values */
  if (maxu(absXm1, absYm1) >= 0x7F7FFFFF) {
    printf("is special\n");
    Min = minu(absX, absY);
    Max = maxu(absX, absY);
    if (Max > 0x7F800000 || (Min == 0 && Max == 0x7F800000)) {
      if (((absX > 0x7F800000) && (X & 0x00400000) == 0) ||
          ((absY > 0x7F800000) && (Y & 0x00400000) == 0) ||
          (Min == 0 && Max == 0x7F800000)) {
        Flags |= V_FLAG;
      }
      /* return qNaN with payload encoded in last 22 bits */
      return Inf | 0x00400000 | Max;
    }
    if (Max != 0x7F800000) {
      return Sr;
    }
    return Inf;
  }
  printf("is normal\n");
  Ex = absX >> 23;
  Ey = absY >> 23;
  nx = absX >= 0x00800000;
  ny = absY >= 0x00800000;
  MX = maxu(nlz32(absX), 8);
  MY = maxu(nlz32(absY), 8);
  printf("Ex=%d, Ey=%d, nx=%d, ny=%d, MX=%d, MY=%d\n",
         Ex, Ey, nx, ny, MX, MY);
  mpX = (X << MX) | 0x80000000;
  mpY = (Y << MY) | 0x80000000;
  mul32(mpX, mpY, &highS, &lowS);
  printf("mpX=0x%08X, mpY=0x%08X, highS=0x%08X, lowS=0x%08X\n",
         mpX, mpY, highS, lowS);
  c = highS >= 0x80000000;
  Dm1 = (((Ex - nx) + (Ey - ny)) - ((MX + MY) + 110)) + c;
  printf("c=%d, Dm1=%d\n", c, Dm1);
#if 0
  if (Dm1 & 0x80000000) {
    rshift = minu(~Dm1 + 1, 32);
    printf("rshift = %d\n", rshift);
    if (rshift == 32) {
      if (lowS != 0 || highS != 0) {
        Flags |= X_FLAG;
      }
      lowS = 0;
      highS = 0;
    } else {
      if (lowS & ((1 << rshift) - 1)) {
        Flags |= X_FLAG;
      }
      lowS = ((highS >> (32 - rshift)) << (32 - rshift)) | (lowS >> rshift);
      highS = highS >> rshift;
    }
    Dm1 = 0;
    Flags |= U_FLAG;
  }
#endif
  if (Dm1 >= 0xFE) {
    Flags |= O_FLAG | X_FLAG;
    return Inf;
  }
  lowT = (lowS != 0);
  G = (highS >> (6 + c)) & 1;
  M = highS >> (7 + c);
  highT = (highS << (26 - c)) != 0;
  printf("lowT=%d, G=%d, M=0x%08X, highT=%d\n", lowT, G, M, highT);
  M_OR_lowT = M | lowT;
  B = G & (M_OR_lowT | highT);
  printf("M_OR_lowT=0x%08X, B=0x%08X\n", M_OR_lowT, B);
  if (G | highT | lowT) {
    Flags |= X_FLAG;
  }
  Res = ((Sr | (Dm1 << 23)) + M) + B;
  printf("return 0x%08X [%e]\n", Res, *(float *)&Res);
  return Res;
}


Word int_fdiv_1(Word X, Word Y) {
  return 0;
}


Word int_fsqrt_1(Word X) {
  Word Xm1;
  Word Ex, nx, MX, mpX, Dm1;
  Word c;
  Word Q, R, S;
  int i, T;

  Xm1 = X - 1;
  /* handle special values */
  if (Xm1 >= 0x7F7FFFFF) {
    if (X <= 0x7F800000 || X == 0x80000000) {
      return X;
    } else {
      if ((X & 0x00400000) == 0 ||
          (X > 0x80000000 && X < 0xFF800000)) {
        Flags |= V_FLAG;
      }
      /* return qNaN with payload encoded in last 22 bits */
      return 0x7FC00000 | X;
    }
  }
  Ex = X >> 23;
  nx = X >= 0x800000;
  MX = max(nlz32(X), 8);
  mpX = (X << MX) | 0x80000000;
  Dm1 = ((Ex - nx) + (134 - MX)) >> 1;
  c = ((Ex - nx) - MX) & 1;
  Q = 0x1000000;
  R = (mpX >> (7 - c)) - Q;
  S = 0x1000000;
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
    Flags |= X_FLAG;
  }
  return (Dm1 << 23) + ((Q >> 1) + (Q & 1));
}


/**************************************************************/


Word int_fadd_2(Word X, Word Y) {
  return 0;
}


Word int_fsub_2(Word X, Word Y) {
  return 0;
}


Word int_fmul_2(Word X, Word Y) {
  Word sX;
  Word eX;
  Word mX;
  Word sY;
  Word eY;
  Word mY;

  sX = X & 0x80000000;
  eX = (X >> 23) & 0xFF;
  mX = X & 0x7FFFFF;
  sY = Y & 0x80000000;
  eY = (Y >> 23) & 0xFF;
  mY = Y & 0x7FFFFF;
  return 0;
}


Word int_fdiv_2(Word X, Word Y) {
  return 0;
}


Word int_fsqrt_2(Word X) {
  Word sX;
  Word eX;
  Word mX;
  Word nlz;
  Word q;

  sX = X & 0x80000000;
  eX = (X >> 23) & 0xFF;
  mX = X & 0x7FFFFF;
  if (eX == 0 && mX == 0) {
    return X;
  }
  if (sX == 0 && eX == 0xFF && mX == 0) {
    return X;
  }
  if (sX != 0 && eX != 0xFF) {
    Flags |= V_FLAG;
    return 0x7FC00000;
  }
  if (eX == 0xFF) {
    if ((mX & 0x400000) == 0) {
      Flags |= V_FLAG;
    }
    return 0x7FC00000 | X;
  }
  if (eX == 0) {
    nlz = nlz32(X);
    q = mX << nlz;
  } else {
    nlz = 0;
    q = mX | 0x80000000;
  }
  if ((eX & 1) != (nlz & 1)) {
  }
  printf("q = 0x%08X, nlz = %d\n", q, nlz);
  return 0x12345678;
}


/**************************************************************/


float fadd(float x, float y) {
  Word X, Y, Z;

  X = *(Word *)&x;
  Y = *(Word *)&y;
  Z = int_fadd(X, Y);
  return *(float *)&Z;
}


float fsub(float x, float y) {
  Word X, Y, Z;

  X = *(Word *)&x;
  Y = *(Word *)&y;
  Z = int_fsub(X, Y);
  return *(float *)&Z;
}


float fmul(float x, float y) {
  Word X, Y, Z;

  X = *(Word *)&x;
  Y = *(Word *)&y;
  Z = int_fmul(X, Y);
  return *(float *)&Z;
}


float fdiv(float x, float y) {
  Word X, Y, Z;

  X = *(Word *)&x;
  Y = *(Word *)&y;
  Z = int_fdiv(X, Y);
  return *(float *)&Z;
}


float fsqrt(float x) {
  Word X, Z;

  X = *(Word *)&x;
  Z = int_fsqrt(X);
  return *(float *)&Z;
}


/**************************************************************/


void basicTests(void) {
  float x, y, r, z;

  x = 1.0;
  y = 2.0;
  r = x + y;
  z = fadd(x, y);
  printf("%13.6e + %13.6e : ref = %13.6e, new = %13.6e\n", x, y, r, z);

  x = 1.0;
  y = 2.0;
  r = x - y;
  z = fsub(x, y);
  printf("%13.6e - %13.6e : ref = %13.6e, new = %13.6e\n", x, y, r, z);

  x = 1.0;
  y = 2.0;
  r = x * y;
  z = fmul(x, y);
  printf("%13.6e * %13.6e : ref = %13.6e, new = %13.6e\n", x, y, r, z);

  x = 123.0;
  y = 123.0;
  r = x * y;
  z = fmul(x, y);
  printf("%13.6e * %13.6e : ref = %13.6e, new = %13.6e\n", x, y, r, z);

  x = 1.0;
  y = 2.0;
  r = x / y;
  z = fdiv(x, y);
  printf("%13.6e / %13.6e : ref = %13.6e, new = %13.6e\n", x, y, r, z);

  x = 15129.0;
  y = 123.0;
  r = x / y;
  z = fdiv(x, y);
  printf("%13.6e / %13.6e : ref = %13.6e, new = %13.6e\n", x, y, r, z);

  x = 2.0;
  r = sqrt(x);
  z = fsqrt(x);
  printf("sqrt(%13.6e)           : ref = %13.6e, new = %13.6e\n", x, r, z);

  x = 15129.0;
  r = sqrt(x);
  z = fsqrt(x);
  printf("sqrt(%13.6e)           : ref = %13.6e, new = %13.6e\n", x, r, z);

  {
  Word X, Y, Z;
  /* +9F.151295  +83.600002  => +A4.027044 .....  expected +A4.027044 ....x */
  printf("------------------------------------------------------------\n");
  printf("+9F.151295  +83.600002  => "
         "+A4.027044 .....  expected +A4.027044 ....x\n");
  Flags = 0;
  X = 0x4F951295;
  Y = 0x41E00002;
  Z = int_fmul(X, Y);
  dumpOpsAndRes(X, Y, Z, Flags);
  /* +26.7F7FFB  -4D.0002FE  => -FF.000000 ..o.x  expected -00.0007FC ...ux */
  printf("------------------------------------------------------------\n");
  printf("+26.7F7FFB  -4D.0002FE  => "
         "-FF.000000 ..o.x  expected -00.0007FC ...ux\n");
  Flags = 0;
  X = 0x137F7FFB;
  Y = 0xA68002FE;
  Z = int_fmul(X, Y);
  dumpOpsAndRes(X, Y, Z, Flags);
  /* +00.000001  +00.000001  => +00.000800 ...ux  expected +00.000000 ...ux */
  printf("------------------------------------------------------------\n");
  printf("+00.000001  +00.000001  => "
         "+00.000800 ...ux  expected +00.000000 ...ux\n");
  Flags = 0;
  X = 0x00000001;
  Y = 0x00000001;
  Z = int_fmul(X, Y);
  dumpOpsAndRes(X, Y, Z, Flags);
  }
}


/**************************************************************/


#define LINE_SIZE	200


void fpTests(char op) {
  char line[LINE_SIZE];
  char *endptr;
  Word X, Y, Z;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    switch (op) {
      case '+':
        endptr = line;
        X = strtoul(endptr, &endptr, 16);
        Y = strtoul(endptr, &endptr, 16);
        Flags = 0;
        Z = int_fadd(X, Y);
        printf("%08X %08X %08X %02X\n", X, Y, Z, Flags);
        break;
      case '-':
        endptr = line;
        X = strtoul(endptr, &endptr, 16);
        Y = strtoul(endptr, &endptr, 16);
        Flags = 0;
        Z = int_fsub(X, Y);
        printf("%08X %08X %08X %02X\n", X, Y, Z, Flags);
        break;
      case '*':
        endptr = line;
        X = strtoul(endptr, &endptr, 16);
        Y = strtoul(endptr, &endptr, 16);
        Flags = 0;
        Z = int_fmul(X, Y);
        printf("%08X %08X %08X %02X\n", X, Y, Z, Flags);
        break;
      case '/':
        endptr = line;
        X = strtoul(endptr, &endptr, 16);
        Y = strtoul(endptr, &endptr, 16);
        Flags = 0;
        Z = int_fdiv(X, Y);
        printf("%08X %08X %08X %02X\n", X, Y, Z, Flags);
        break;
      case 'r':
        endptr = line;
        X = strtoul(endptr, &endptr, 16);
        Flags = 0;
        Z = int_fsqrt(X);
        printf("%08X %08X %02X\n", X, Z, Flags);
        break;
      default:
        fprintf(stderr, "\n**** Error: illegal operation '%c' ****\n", op);
        exit(1);
        break;
    }
  }
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s [-add|-sub|-mul|-div|-sqrt]\n", myself);
}


int main(int argc, char *argv[]) {
  if (argc == 1) {
    nlz32_test();
    mul32_test();
    basicTests();
    return 0;
  }
  if (argc != 2) {
    usage(argv[0]);
    return 1;
  }
  if (strcmp(argv[1], "-add") == 0) {
    fpTests('+');
    return 0;
  }
  if (strcmp(argv[1], "-sub") == 0) {
    fpTests('-');
    return 0;
  }
  if (strcmp(argv[1], "-mul") == 0) {
    fpTests('*');
    return 0;
  }
  if (strcmp(argv[1], "-div") == 0) {
    fpTests('/');
    return 0;
  }
  if (strcmp(argv[1], "-sqrt") == 0) {
    fpTests('r');
    return 0;
  }
  usage(argv[0]);
  return 1;
}
