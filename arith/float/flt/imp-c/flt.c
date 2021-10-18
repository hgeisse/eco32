/*
 * flt.c -- implementation and test of conversion to float
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../include/fp.h"


typedef enum { false = 0, true = 1 } Bool;


/**************************************************************/

/*
 * display functions
 */


void dump(_FP_Word w) {
  printf("0x%08X = [%c%02X.%06X]",
         w, _FP_SGN(w) ? '-' : '+', _FP_EXP(w), _FP_FRC(w));
}


void showClass(_FP_Word w) {
  _FP_Word e;
  _FP_Word f;

  e = _FP_EXP(w);
  f = _FP_FRC(w);
  printf("<%s", _FP_SGN(w) ? "-" : "+");
  switch (e) {
    case 0:
      if (f == 0) {
        printf("Zero");
      } else {
        printf("Subn");
      }
      break;
    case 255:
      if (f == 0) {
        printf("Infn");
      } else {
        if (f & 0x00400000) {
          printf("qNaN");
        } else {
          printf("sNaN");
        }
      }
      break;
    default:
      printf("Nrml");
      break;
  }
  printf(">");
}


void showValue(_FP_Word w) {
  _FP_Union X;

  X.w = w;
  printf("%e", X.f);
}


void show(char *name, _FP_Word w) {
  printf("%s = ", name);
  dump(w);
  printf(" = ");
  showClass(w);
  printf(" = ");
  showValue(w);
}


void showFlags(_FP_Word flags) {
  printf("%c", (flags & _FP_V_FLAG) ? 'v' : '.');
  printf("%c", (flags & _FP_I_FLAG) ? 'i' : '.');
  printf("%c", (flags & _FP_O_FLAG) ? 'o' : '.');
  printf("%c", (flags & _FP_U_FLAG) ? 'u' : '.');
  printf("%c", (flags & _FP_X_FLAG) ? 'x' : '.');
}


void showInt(char *name, int n) {
  printf("%s = 0x%08X = %d", name, n, n);
}


/**************************************************************/

/*
 * 32-bit leading-zero counter, implementation
 */


typedef unsigned char Byte;


Byte encode2(Byte two_bits) {
  Byte r;

  switch (two_bits) {
    case 0:
      r = 2;
      break;
    case 1:
      r = 1;
      break;
    case 2:
      r = 0;
      break;
    case 3:
      r = 0;
      break;
  }
  return r;
}


Byte combine2(Byte left, Byte right) {
  Byte r;

  if ((left & 2) != 0 && (right & 2) != 0) {
    r = 4;
  } else
  if ((left & 2) != 0) {
    r = right | 2;
  } else {
    r = left;
  }
  return r;
}


Byte combine3(Byte left, Byte right) {
  Byte r;

  if ((left & 4) != 0 && (right & 4) != 0) {
    r = 8;
  } else
  if ((left & 4) != 0) {
    r = right | 4;
  } else {
    r = left;
  }
  return r;
}


Byte combine4(Byte left, Byte right) {
  Byte r;

  if ((left & 8) != 0 && (right & 8) != 0) {
    r = 16;
  } else
  if ((left & 8) != 0) {
    r = right | 8;
  } else {
    r = left;
  }
  return r;
}


Byte combine5(Byte left, Byte right) {
  Byte r;

  if ((left & 16) != 0 && (right & 16) != 0) {
    r = 32;
  } else
  if ((left & 16) != 0) {
    r = right | 16;
  } else {
    r = left;
  }
  return r;
}


int lzc32(_FP_Word m) {
  Byte lz1_f, lz1_e, lz1_d, lz1_c, lz1_b, lz1_a, lz1_9, lz1_8;
  Byte lz1_7, lz1_6, lz1_5, lz1_4, lz1_3, lz1_2, lz1_1, lz1_0;
  Byte lz2_7, lz2_6, lz2_5, lz2_4, lz2_3, lz2_2, lz2_1, lz2_0;
  Byte lz3_3, lz3_2, lz3_1, lz3_0;
  Byte lz4_1, lz4_0;
  Byte lz5_0;

  /* stage 1: encode 2 bits */
  /* representing the LZC of 2 bits */
  lz1_f = encode2((m >> 30) & 0x03);
  lz1_e = encode2((m >> 28) & 0x03);
  lz1_d = encode2((m >> 26) & 0x03);
  lz1_c = encode2((m >> 24) & 0x03);
  lz1_b = encode2((m >> 22) & 0x03);
  lz1_a = encode2((m >> 20) & 0x03);
  lz1_9 = encode2((m >> 18) & 0x03);
  lz1_8 = encode2((m >> 16) & 0x03);
  lz1_7 = encode2((m >> 14) & 0x03);
  lz1_6 = encode2((m >> 12) & 0x03);
  lz1_5 = encode2((m >> 10) & 0x03);
  lz1_4 = encode2((m >>  8) & 0x03);
  lz1_3 = encode2((m >>  6) & 0x03);
  lz1_2 = encode2((m >>  4) & 0x03);
  lz1_1 = encode2((m >>  2) & 0x03);
  lz1_0 = encode2((m >>  0) & 0x03);
  /* stage 2: combine 2+2 bits to 3 bits */
  /* representing the LZC of 4 bits */
  lz2_7 = combine2(lz1_f, lz1_e);
  lz2_6 = combine2(lz1_d, lz1_c);
  lz2_5 = combine2(lz1_b, lz1_a);
  lz2_4 = combine2(lz1_9, lz1_8);
  lz2_3 = combine2(lz1_7, lz1_6);
  lz2_2 = combine2(lz1_5, lz1_4);
  lz2_1 = combine2(lz1_3, lz1_2);
  lz2_0 = combine2(lz1_1, lz1_0);
  /* stage 3: combine 3+3 bits to 4 bits */
  /* representing the LZC of 8 bits */
  lz3_3 = combine3(lz2_7, lz2_6);
  lz3_2 = combine3(lz2_5, lz2_4);
  lz3_1 = combine3(lz2_3, lz2_2);
  lz3_0 = combine3(lz2_1, lz2_0);
  /* stage 4: combine 4+4 bits to 5 bits */
  /* representing the LZC of 16 bits */
  lz4_1 = combine4(lz3_3, lz3_2);
  lz4_0 = combine4(lz3_1, lz3_0);
  /* stage 5: combine 5+5 bits to 6 bits */
  /* representing the LZC of 32 bits */
  lz5_0 = combine5(lz4_1, lz4_0);
  return lz5_0;
}


/**************************************************************/

/*
 * 32-bit leading-zero counter, internal reference
 */


int lzc32_ref(_FP_Word m) {
  int i;

  for (i = 31; i >= 0; i--) {
    if (m & (1 << i)) {
      return 31 - i;
    }
  }
  return 32;
}


/**************************************************************/


void check_lzc32(void) {
  int i, j;
  _FP_Word m;
  int lz, lz_ref;
  long count, errors;

  printf("Part 1: single bit test\n");
  count = 0;
  errors = 0;
  for (i = 0; i < 32; i++) {
    m = (1 << i);
    lz = lzc32(m);
    lz_ref = lzc32_ref(m);
    count++;
    if (lz != lz_ref) {
      errors++;
    }
  }
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
  printf("Part 2: small value test\n");
  count = 0;
  errors = 0;
  for (i = 0; i < (1 << 20); i++) {
    m = i;
    lz = lzc32(m);
    lz_ref = lzc32_ref(m);
    count++;
    if (lz != lz_ref) {
      errors++;
    }
  }
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
  printf("Part 3: full range test\n");
  count = 0;
  errors = 0;
  for (i = 0; i < 256; i++) {
    for (j = 0; j < (1 << 24); j += 257) {
      m = (i << 24) | j;
      lz = lzc32(m);
      lz_ref = lzc32_ref(m);
      count++;
      if (lz != lz_ref) {
        errors++;
      }
    }
  }
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
}


/**************************************************************/

/*
 * floating-point float function, implementation
 */


#define DEVELOPING	0

#if DEVELOPING
_FP_Word fake_fpFlt(int x) {
  extern _FP_Word Flags;
  extern _FP_Word Flags_ref;
  extern _FP_Word fpFlt_ref(int x);
  _FP_Word Flags_save;
  _FP_Word z;

  Flags_save = Flags_ref;
  Flags_ref = 0;
  z = fpFlt_ref(x);
  Flags = Flags_ref;
  Flags_ref = Flags_save;
  return z;
}
#endif


#define genZERO		((_FP_Word) 0x00000000)
#define genNAN		((_FP_Word) 0x7FC00000)
#define genINF		((_FP_Word) 0x7F800000)

#define QUIET_BIT	((_FP_Word) 0x00400000)
#define IS_QUIET(f)	((f & QUIET_BIT) != 0)

#define IS_ZERO(e,f)	((e) ==   0 && (f) ==   0)
#define IS_SUB(e,f)	((e) ==   0 && (f) !=   0)
#define IS_INF(e,f)	((e) == 255 && (f) ==   0)
#define IS_NAN(e,f)	((e) == 255 && (f) !=   0)
#define IS_NORM(e)	((e) !=   0 && (e) != 255)
#define IS_NUM(e,f)	(IS_NORM(e) || IS_SUB(e, f))


Bool debug = false;

_FP_Word Flags = 0;


_FP_Word fpFlt(int x) {
  _FP_Word sz, ez, fz;
  _FP_Word xabs;
  int lx;
  _FP_Word m;
  _FP_Word z;
  Bool round, sticky, odd, incr;

  if (debug) {
    printf("------------------------------------------------\n");
    showInt("x", x);
    printf("\n");
  }
  if (x == 0) {
    sz = 0;
    ez = _FP_EXP(genZERO);
    fz = _FP_FRC(genZERO);
    z = _FP_FLT(sz, ez, fz);
  } else {
    if (x < 0) {
      xabs = -x;
      sz = 1;
    } else {
      xabs = x;
      sz = 0;
    }
    lx = lzc32(xabs);
    if (debug) {
      printf("xabs = 0x%08X, lx = %d\n", xabs, lx);
    }
    m = xabs << lx;
    ez = 158 - lx;
    if (debug) {
      printf("m = 0x%08X, ez = %d\n", m, ez);
    }
    fz = (m & ~(1 << 31)) >> 8;
    round = ((m & (1 << 7)) != 0);
    sticky = ((m & 0x7F) != 0);
    odd = ((fz & 1) != 0);
    incr = round && (sticky || odd);
    if (debug) {
      printf("round = %d, sticky = %d, odd = %d => incr = %d\n",
             round, sticky, odd, incr);
    }
    z = _FP_FLT(sz, ez, fz);
    if (incr) {
      z++;
    }
    if (round || sticky) {
      Flags |= _FP_X_FLAG;
    }
  }
  if (debug) {
    show("z", z);
    printf("    ");
    showFlags(Flags);
    printf("\n");
    printf("------------------------------------------------\n");
  }
  return z;
}


/**************************************************************/

/*
 * floating-point float function, internal reference
 */


#include "softfloat.h"


_FP_Word Flags_ref = 0;


_FP_Word fpFlt_ref(int x) {
  float32_t Z;

  softfloat_detectTininess = softfloat_tininess_beforeRounding;
  softfloat_roundingMode = softfloat_round_near_even;
  softfloat_exceptionFlags = 0;
  Z = i32_to_f32(x);
  Flags_ref |=
    ((softfloat_exceptionFlags & softfloat_flag_invalid)   ? _FP_V_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_infinite)  ? _FP_I_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_overflow)  ? _FP_O_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_underflow) ? _FP_U_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_inexact)   ? _FP_X_FLAG : 0);
  return Z.v;
}


/**************************************************************/

/*
 * floating-point comparison
 */


Bool compareEqual(_FP_Word x, _FP_Word y) {
  if (x == y) {
    return true;
  }
  if (_FP_EXP(x) == 255 && _FP_FRC(x) != 0 &&
      _FP_EXP(y) == 255 && _FP_FRC(y) != 0) {
    /* both are NaNs: compare quiet bits, but not signs nor payloads */
    return (x & 0x00400000) == (y & 0x00400000);
  }
  return false;
}


/**************************************************************/

/*
 * check the floating-point operation with simple values
 */


void check_simple(void) {
  int i;
  int x;
  _FP_Word z, r;
  Bool error;

  /* [1, 20] */
  printf("------------------------------------------------\n");
  printf("interval = [1, 20]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    x = i;
    Flags = 0;
    z = fpFlt(x);
    Flags_ref = 0;
    r = fpFlt_ref(x);
    show("r", r);
    printf("    ");
    showFlags(Flags_ref);
    printf("\n");
    if (!compareEqual(z, r)) {
      error = true;
      printf("value wrong");
    } else {
      printf("value ok");
    }
    printf(", ");
    if (Flags != Flags_ref) {
      error = true;
      printf("flags wrong");
    } else {
      printf("flags ok");
    }
    printf("\n");
    printf("------------------------------------------------\n");
    if (error) {
      printf("summary: stopped on first error\n");
      return;
    }
  }
  /* [-1, -20] */
  printf("------------------------------------------------\n");
  printf("interval = [-1, -20]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    x = -i;
    Flags = 0;
    z = fpFlt(x);
    Flags_ref = 0;
    r = fpFlt_ref(x);
    show("r", r);
    printf("    ");
    showFlags(Flags_ref);
    printf("\n");
    if (!compareEqual(z, r)) {
      error = true;
      printf("value wrong");
    } else {
      printf("value ok");
    }
    printf(", ");
    if (Flags != Flags_ref) {
      error = true;
      printf("flags wrong");
    } else {
      printf("flags ok");
    }
    printf("\n");
    printf("------------------------------------------------\n");
    if (error) {
      printf("summary: stopped on first error\n");
      return;
    }
  }
  /* powers of 2 */
  printf("------------------------------------------------\n");
  printf("powers of 2\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 0; i <= 31; i++) {
    x = (1 << i);
    Flags = 0;
    z = fpFlt(x);
    Flags_ref = 0;
    r = fpFlt_ref(x);
    show("r", r);
    printf("    ");
    showFlags(Flags_ref);
    printf("\n");
    if (!compareEqual(z, r)) {
      error = true;
      printf("value wrong");
    } else {
      printf("value ok");
    }
    printf(", ");
    if (Flags != Flags_ref) {
      error = true;
      printf("flags wrong");
    } else {
      printf("flags ok");
    }
    printf("\n");
    printf("------------------------------------------------\n");
    if (error) {
      printf("summary: stopped on first error\n");
      return;
    }
  }
  printf("summary: no errors\n");
}


/**************************************************************/

/*
 * check the floating-point operation with selected values
 */


typedef struct {
  int x;
} _Int_Single;


_Int_Single singles[] = {
  /*
   * special values
   */
  { 0x00000000 },
  { 0x00000001 },
  { 0x00000002 },
  { 0x00000004 },
  { 0x00000008 },
  { 0x00000010 },
  { 0x00000100 },
  { 0x00001000 },
  { 0x00010000 },
  { 0x00100000 },
  { 0x01000000 },
  { 0x10000000 },
  { 0x20000000 },
  { 0x40000000 },
  { 0x80000000 },
  { 0xFFFFFFFF },
  { 0x00800000 },
  { 0x01000000 },
  { 0x02000000 },
  { 0x02000001 },
  { 0x02000002 },
  { 0x02000003 },
  { 0x02000004 },
  { 0x02000005 },
  { 0x02000006 },
  { 0x02000007 },
};

int numSingles = sizeof(singles)/sizeof(singles[0]);


void check_selected(void) {
  int x;
  _FP_Word z, r;
  int i;
  Bool error;

  error = false;
  for (i = 0; i < numSingles; i++) {
    x = singles[i].x;
    Flags = 0;
    z = fpFlt(x);
    Flags_ref = 0;
    r = fpFlt_ref(x);
    show("r", r);
    printf("    ");
    showFlags(Flags_ref);
    printf("\n");
    if (!compareEqual(z, r)) {
      error = true;
      printf("value wrong");
    } else {
      printf("value ok");
    }
    printf(", ");
    if (Flags != Flags_ref) {
      error = true;
      printf("flags wrong");
    } else {
      printf("flags ok");
    }
    printf("\n");
    printf("------------------------------------------------\n");
    if (error) {
      printf("summary: stopped on first error\n");
      return;
    }
  }
  printf("summary: no errors\n");
}


/**************************************************************/

/*
 * check the floating-point operation in different intervals
 */


#define SKIP_MASK       0x00003F00


void check_intervals(void) {
  int x;
  _FP_Word z, r;
  long count, errors;

  printf("------------------------------------------------\n");
  printf("Part 1: full small interval [0x3FABBBBB, 0x3FABCDEF]\n");
  count = 0;
  errors = 0;
  x = 0x3FABBBBB;
  do {
    Flags = 0;
    z = fpFlt(x);
    Flags_ref = 0;
    r = fpFlt_ref(x);
    count++;
    if (!compareEqual(z, r) || Flags != Flags_ref) {
      if (errors == 0) {
        printf("first error: x = 0x%08X\n", x);
        printf("             z = 0x%08X, r = 0x%08X\n", z, r);
      }
      errors++;
    }
    x++;
  } while (x <= 0x3FABCDEF);
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
  printf("------------------------------------------------\n");
  printf("Part 2: sparse bigger interval [0x3F000000, 0x40000000]\n");
  count = 0;
  errors = 0;
  x = 0x3F000000;
  do {
    Flags = 0;
    z = fpFlt(x);
    Flags_ref = 0;
    r = fpFlt_ref(x);
    count++;
    if (!compareEqual(z, r) || Flags != Flags_ref) {
      if (errors == 0) {
        printf("first error: x = 0x%08X\n", x);
        printf("             z = 0x%08X, r = 0x%08X\n", z, r);
      }
      errors++;
    }
    x += 3001;
  } while (x <= 0x40000000);
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
  printf("------------------------------------------------\n");
  printf("Part 3: full range, big gaps\n");
  count = 0;
  errors = 0;
  x = 0;
  do {
    if ((x & 0x0FFFFFFF) == 0) {
      printf("reached test loop 0x%08X\n", x);
    }
    if ((x & SKIP_MASK) != 0x00000000 &&
        (x & SKIP_MASK) != SKIP_MASK) {
      x |= SKIP_MASK;
    }
    Flags = 0;
    z = fpFlt(x);
    Flags_ref = 0;
    r = fpFlt_ref(x);
    count++;
    if (!compareEqual(z, r) || Flags != Flags_ref) {
      if (errors == 0) {
        printf("first error: x = 0x%08X\n", x);
        printf("             z = 0x%08X, r = 0x%08X\n", z, r);
      }
      errors++;
    }
    x++;
  } while (x != 0);
  printf("number of tests  = %ld\n", count);
  printf("number of errors = %ld\n", errors);
  printf("------------------------------------------------\n");
}


/**************************************************************/

/*
 * make the floating-point operation available as a service
 * (used to check against an external reference implementation)
 */


#define LINE_SIZE       200


void server(void) {
  char line[LINE_SIZE];
  char *endptr;
  int x;
  _FP_Word z;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    x = strtoul(endptr, &endptr, 16);
    Flags = 0;
    z = fpFlt(x);
    printf("%08X %08X %02X\n", x, z, Flags);
  }
}


/**************************************************************/

/*
 * main program
 */


void usage(char *myself) {
  printf("usage: %s -lzc32 | -simple | "
         "-selected | -intervals | -server\n",
         myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-lzc32") == 0) {
    printf("Check 32-bit leading-zero counter\n");
    check_lzc32();
  } else
  if (strcmp(argv[1], "-simple") == 0) {
    printf("Check simple test cases\n");
    debug = true;
    check_simple();
  } else
  if (strcmp(argv[1], "-selected") == 0) {
    printf("Check selected test cases\n");
    debug = true;
    check_selected();
  } else
  if (strcmp(argv[1], "-intervals") == 0) {
    printf("Check different intervals\n");
    check_intervals();
  } else
  if (strcmp(argv[1], "-server") == 0) {
    server();
  } else {
    usage(argv[0]);
  }
  return 0;
}
