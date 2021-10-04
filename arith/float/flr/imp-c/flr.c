/*
 * flr.c -- implementation and test of conversion to integer
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../include/fp.h"


typedef enum { false = 0, true = 1 } Bool;


/**************************************************************/


Bool truncate;		/* floor() vs. trunc() */


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
 * floating-point floor function, implementation
 */


#define DEVELOPING	0

#if DEVELOPING
int fake_fpFlr(_FP_Word x) {
  extern _FP_Word Flags;
  extern _FP_Word Flags_ref;
  extern int fpFlr_ref(_FP_Word x);
  _FP_Word Flags_save;
  int z;

  Flags_save = Flags_ref;
  Flags_ref = 0;
  z = fpFlr_ref(x);
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


int fpFlr(_FP_Word x) {
  _FP_Word sx, ex, fx;
  _FP_Word mxnorm;
  int exnorm;
  int z;
  Bool decr;

  if (debug) {
    printf("------------------------------------------------\n");
    show("x", x);
    printf("\n");
  }
  sx = _FP_SGN(x);
  ex = _FP_EXP(x);
  fx = _FP_FRC(x);
  if (debug) {
    printf("x: sx = %d, ex = %3d, fx = 0x%06X\n", sx, ex, fx);
  }
  if (IS_ZERO(ex, fx)) {
    z = 0;
  } else
  if (IS_INF(ex, fx)) {
    z = 0x80000000;
    Flags |= _FP_V_FLAG;
  } else
  if (IS_NAN(ex, fx)) {
    z = 0x80000000;
    Flags |= _FP_V_FLAG;
  } else {
    if (!IS_NUM(ex, fx)) {
      fprintf(stderr, "FATAL INTERNAL ERROR 1\n");
      exit(1);
    }
    mxnorm = (1 << 23) | fx;
    exnorm = ex - 127;
    if (debug) {
      printf("mxnorm = 0x%08X, exnorm = %d\n", mxnorm, exnorm);
    }
    if (exnorm < 0) {
      z = (sx && !truncate) ? 0xFFFFFFFF : 0x00000000;
      Flags |= _FP_X_FLAG;
    } else
    if (exnorm > 31) {
      z = 0x80000000;
      Flags |= _FP_V_FLAG;
    } else
    if (exnorm == 31) {
      z = 0x80000000;
      if (!sx || fx != 0) {
        Flags |= _FP_V_FLAG;
      }
    } else {
      if (exnorm < 23) {
        z = mxnorm >> (23 - exnorm);
        if ((mxnorm << (9 + exnorm)) != 0) {
          Flags |= _FP_X_FLAG;
          decr = truncate ? 0 : 1;
        } else {
          decr = 0;
        }
      } else {
        z = mxnorm << (exnorm - 23);
        decr = 0;
      }
      if (sx) {
        z = -z - decr;
      }
    }
  }
  if (debug) {
    showInt("z", z);
    printf("    ");
    showFlags(Flags);
    printf("\n");
    printf("------------------------------------------------\n");
  }
  return z;
}


/**************************************************************/

/*
 * floating-point floor function, internal reference
 */


#include "softfloat.h"


_FP_Word Flags_ref = 0;


int fpFlr_ref(_FP_Word x) {
  float32_t X;
  int z;

  X.v = x;
  softfloat_exceptionFlags = 0;
  z = f32_to_i32(X,
                 truncate ? softfloat_round_minMag : softfloat_round_min,
                 true);
  Flags_ref |=
    ((softfloat_exceptionFlags & softfloat_flag_invalid)   ? _FP_V_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_infinite)  ? _FP_I_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_overflow)  ? _FP_O_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_underflow) ? _FP_U_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_inexact)   ? _FP_X_FLAG : 0);
  return z;
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
  _FP_Word x;
  int z, r;
  _FP_Union u;
  Bool error;

  /* [1.0, 20.0] */
  printf("------------------------------------------------\n");
  printf("interval = [1.0, 20.0]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    u.f = (float) i;
    x = u.w;
    Flags = 0;
    z = fpFlr(x);
    Flags_ref = 0;
    r = fpFlr_ref(x);
    showInt("r", r);
    printf("    ");
    showFlags(Flags_ref);
    printf("\n");
    if (z != r) {
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
  /* [0.10, 2.00] */
  printf("------------------------------------------------\n");
  printf("interval = [0.10, 2.00]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    u.f = (float) i / (float) 10.0;
    x = u.w;
    Flags = 0;
    z = fpFlr(x);
    Flags_ref = 0;
    r = fpFlr_ref(x);
    showInt("r", r);
    printf("    ");
    showFlags(Flags_ref);
    printf("\n");
    if (z != r) {
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
  _FP_Word x;
} _FP_Single;


_FP_Single singles[] = {
  /*
   * special values
   */
  /* { zero, inf } */
  { 0x00000000 },
  { 0x7F800000 },
  { 0x80000000 },
  { 0xFF800000 },
  /* { sNan, qNan } */
  { 0x7F800005 },
  { 0x7FC00005 },
  { 0xFF800005 },
  { 0xFFC00005 },
  /*
   * exponents
   */
  { 0x3E000000 },
  { 0x3E800000 },
  { 0x3F000000 },
  { 0x3F800000 },
  { 0x40000000 },
  { 0x40800000 },
  { 0x41000000 },
  { 0x41800000 },
  { 0x42000000 },
  { 0x42800000 },
  { 0x43000000 },
  { 0x43800000 },
  { 0x44000000 },
  { 0x44800000 },
  { 0x45000000 },
  { 0x45800000 },
  { 0x46000000 },
  { 0x46800000 },
  { 0x47000000 },
  { 0x47800000 },
  { 0x48000000 },
  { 0x48800000 },
  { 0x49000000 },
  { 0x49800000 },
  { 0x4A000000 },
  { 0x4A800000 },
  { 0x4B000000 },
  { 0x4B800000 },
  { 0x4C000000 },
  { 0x4C800000 },
  { 0x4D000000 },
  { 0x4D800000 },
  { 0x4E000000 },
  { 0x4E800000 },
  { 0x4F000000 },
  { 0x4F800000 },
  { 0x50000000 },
  { 0x50800000 },
  /*
   * other tests
   */
  { 0x4F951295 },
  { 0x4E000000 },
  { 0x45800000 },
  { 0xC5800000 },
  { 0x4F000000 },
  { 0xCF000000 },
};

int numSingles = sizeof(singles)/sizeof(singles[0]);


void check_selected(void) {
  _FP_Word x;
  int z, r;
  int i;
  Bool error;

  error = false;
  for (i = 0; i < numSingles; i++) {
    x = singles[i].x;
    Flags = 0;
    z = fpFlr(x);
    Flags_ref = 0;
    r = fpFlr_ref(x);
    showInt("r", r);
    printf("    ");
    showFlags(Flags_ref);
    printf("\n");
    if (z != r) {
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
  _FP_Word x;
  int z, r;
  long count, errors;

  printf("------------------------------------------------\n");
  printf("Part 1: full small interval [0x3FABBBBB, 0x3FABCDEF]\n");
  count = 0;
  errors = 0;
  x = 0x3FABBBBB;
  do {
    Flags = 0;
    z = fpFlr(x);
    Flags_ref = 0;
    r = fpFlr_ref(x);
    count++;
    if (z != r || Flags != Flags_ref) {
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
    z = fpFlr(x);
    Flags_ref = 0;
    r = fpFlr_ref(x);
    count++;
    if (z != r || Flags != Flags_ref) {
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
    z = fpFlr(x);
    Flags_ref = 0;
    r = fpFlr_ref(x);
    count++;
    if (z != r || Flags != Flags_ref) {
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


#define LINE_SIZE	200


void server(void) {
  char line[LINE_SIZE];
  char *endptr;
  _FP_Word x;
  int z;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    x = strtoul(endptr, &endptr, 16);
    Flags = 0;
    z = fpFlr(x);
    printf("%08X %08X %02X\n", x, z, Flags);
  }
}


/**************************************************************/

/*
 * main program
 */


void usage(char *myself) {
  printf("usage: %s (-floor | -trunc) "
         "(-simple | -selected | -intervals | -server)\n",
         myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc != 3) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-floor") == 0) {
    truncate = false;
  } else
  if (strcmp(argv[1], "-trunc") == 0) {
    truncate = true;
  } else {
    usage(argv[0]);
  }
  if (strcmp(argv[2], "-simple") == 0) {
    printf("Check simple test cases\n");
    debug = true;
    check_simple();
  } else
  if (strcmp(argv[2], "-selected") == 0) {
    printf("Check selected test cases\n");
    debug = true;
    check_selected();
  } else
  if (strcmp(argv[2], "-intervals") == 0) {
    printf("Check different intervals\n");
    check_intervals();
  } else
  if (strcmp(argv[2], "-server") == 0) {
    server();
  } else {
    usage(argv[0]);
  }
  return 0;
}
