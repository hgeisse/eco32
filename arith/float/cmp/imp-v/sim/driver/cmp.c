/*
 * cmp.c -- implementation and test of comparisons
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../include/fp.h"


#define PATH_TO_FPSERVER	"../device/fpserver"
#define LINE_SIZE		200


typedef enum { false = 0, true = 1 } Bool;


/**************************************************************/


#define PRED_EQ		0
#define PRED_NE		1
#define PRED_LE		2
#define PRED_LT		3


int predicate;		/* eq() vs. ne() vs. le() vs. lt() */


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


void showBool(char *name, Bool b) {
  printf("%s = %d = %s", name, b, b ? "true " : "false");
}


/**************************************************************/

/*
 * device communication
 */


#include <stdarg.h>
#include <unistd.h>
#include <signal.h>


pid_t fpserver;

FILE *writeToDevice;
FILE *readFromDevice;


void fatalError(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "FATAL ERROR: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}


void startDevice(char *pathToFPServer) {
  int pipeToDevice[2];
  int pipeToDriver[2];

  if (pipe(pipeToDevice) < 0) {
    fatalError("cannot open pipe");
  }
  if (pipe(pipeToDriver) < 0) {
    fatalError("cannot open pipe");
  }
  fpserver = fork();
  if (fpserver < 0) {
    fatalError("cannot fork");
  }
  if (fpserver == 0) {
    /* device */
    close(pipeToDevice[1]);
    close(pipeToDriver[0]);
    dup2(pipeToDevice[0], 0);
    dup2(pipeToDriver[1], 1);
    close(pipeToDevice[0]);
    close(pipeToDriver[1]);
    execl(pathToFPServer, pathToFPServer, NULL);
    fatalError("cannot exec FP server '%s'", pathToFPServer);
  } else {
    /* driver */
    close(pipeToDevice[0]);
    close(pipeToDriver[1]);
    writeToDevice = fdopen(pipeToDevice[1], "w");
    if (writeToDevice == NULL) {
      fatalError("cannot open writeToDevice");
    }
    readFromDevice = fdopen(pipeToDriver[0], "r");
    if (readFromDevice == NULL) {
      fatalError("cannot open readFromDevice");
    }
  }
}


void stopDevice(void) {
  kill(fpserver, SIGKILL);
}


void sndDevice(int pred, _FP_Word x, _FP_Word y) {
  fprintf(writeToDevice, "%d %08X %08X\n", pred, x, y);
  fflush(writeToDevice);
}


void rcvDevice(Bool *zp, _FP_Word *fp) {
  static Bool firstLine = true;
  char line[LINE_SIZE];
  char *endptr;

  if (fgets(line, LINE_SIZE, readFromDevice) == NULL) {
    fatalError("cannot read answer from simulation");
  }
  if (firstLine) {
    firstLine = false;
    if (strncmp(line, "VCD info: dumpfile", 18) == 0) {
      if (fgets(line, LINE_SIZE, readFromDevice) == NULL) {
        fatalError("cannot read answer from simulation");
      }
    }
  }
  endptr = line;
  *zp = strtoul(endptr, &endptr, 10);
  *fp = strtoul(endptr, &endptr, 16);
}


/**************************************************************/

/*
 * floating-point comparator, implementation
 */


_FP_Word Flags = 0;


Bool fpCmp(_FP_Word x, _FP_Word y) {
  Bool z;

  sndDevice(predicate, x, y);
  rcvDevice(&z, &Flags);
  return z;
}


/**************************************************************/

/*
 * floating-point comparator, internal reference
 */


#include "softfloat.h"


_FP_Word Flags_ref = 0;


Bool fpCmp_ref(_FP_Word x, _FP_Word y) {
  float32_t X, Y;
  Bool Z;

  X.v = x;
  Y.v = y;
  softfloat_detectTininess = softfloat_tininess_beforeRounding;
  softfloat_roundingMode = softfloat_round_near_even;
  softfloat_exceptionFlags = 0;
  switch (predicate) {
    case PRED_EQ:
      Z = f32_eq(X, Y);
      break;
    case PRED_NE:
      /* Note: The following is indeed correct! */
      Z = !f32_eq(X, Y);
      break;
    case PRED_LE:
      /* Note: The following is not equal to "less or equal"! */
      Z = f32_le(X, Y);
      break;
    case PRED_LT:
      Z = f32_lt(X, Y);
      break;
  }
  Flags_ref |=
    ((softfloat_exceptionFlags & softfloat_flag_invalid)   ? _FP_V_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_infinite)  ? _FP_I_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_overflow)  ? _FP_O_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_underflow) ? _FP_U_FLAG : 0) |
    ((softfloat_exceptionFlags & softfloat_flag_inexact)   ? _FP_X_FLAG : 0);
  return Z;
}


/**************************************************************/

/*
 * check the floating-point operation with simple values
 */


void check_simple(void) {
  int i, j;
  _FP_Word x, y;
  Bool z, r;
  _FP_Union u;
  Bool error;

  /* [1.0, 20.0] */
  printf("------------------------------------------------\n");
  printf("interval = [1.0, 20.0]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    for (j = 1; j <= 20; j++) {
      u.f = (float) i;
      x = u.w;
      u.f = (float) j;
      y = u.w;
      Flags = 0;
      z = fpCmp(x, y);
      Flags_ref = 0;
      r = fpCmp_ref(x, y);
      showBool("r", r);
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
  }
  /* [0.10, 2.00] */
  printf("------------------------------------------------\n");
  printf("interval = [0.10, 2.00]\n");
  printf("------------------------------------------------\n");
  error = false;
  for (i = 1; i <= 20; i++) {
    for (j = 1; j <= 20; j++) {
      u.f = (float) i / (float) 10.0;
      x = u.w;
      u.f = (float) j / (float) 10.0;
      y = u.w;
      Flags = 0;
      z = fpCmp(x, y);
      Flags_ref = 0;
      r = fpCmp_ref(x, y);
      showBool("r", r);
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
  }
  printf("summary: no errors\n");
}


/**************************************************************/

/*
 * check the floating-point operation with selected values
 */


typedef struct {
  _FP_Word x;
  _FP_Word y;
} _FP_Pair;


_FP_Pair pairs[] = {
  /*
   * special values
   */
  /* { zero, inf } */
  { 0x00000000, 0x00000000 },
  { 0x7F800000, 0x7F800000 },
  { 0x00000000, 0x7F800000 },
  { 0x7F800000, 0x00000000 },
  /* { sNan, qNan } */
  { 0x7F800005, 0x7F800006 },
  { 0x7FC00005, 0x7FC00006 },
  { 0x7F800005, 0x7FC00006 },
  { 0x7FC00005, 0x7F800006 },
  /* { sNan, zero } */
  { 0x7F800005, 0x00000000 },
  { 0x00000000, 0x7F800006 },
  /* { sNan, inf } */
  { 0x7F800005, 0x7F800000 },
  { 0x7F800000, 0x7F800006 },
  /* { qNan, zero } */
  { 0x7FC00005, 0x00000000 },
  { 0x00000000, 0x7FC00006 },
  /* { qNan, inf } */
  { 0x7FC00005, 0x7F800000 },
  { 0x7F800000, 0x7FC00006 },
  /*
   * normal values, overflow
   */
  { 0x7F7FFFFF, 0x7F7FFFFF },
  { 0x60000000, 0x60000000 },
  { 0x5F800001, 0x5F7FFFFF },
  /*
   * normal values, normal result
   */
  { 0x3F800000, 0x3F800000 },
  { 0x3F800000, 0x40000000 },
  { 0x40000000, 0x40000000 },
  { 0x40000000, 0x40400000 },
  { 0x40400000, 0x40400000 },
  /*
   * normal values, subnormal result
   */
  { 0x1F800000, 0x1F800000 },
  /*
   * subnormal values
   */
  { 0x40000000, 0x00444444 },
  { 0x00444444, 0x40000000 },
  { 0x3F800000, 0x00444444 },
  { 0x00444444, 0x3F800000 },
  { 0x0006274F, 0xDE7FC1FF },
  /*
   * subnormal results
   */
  { 0x26A50F00, 0x1F86A50F },
  { 0x1F86A50F, 0x1F86A50F },
  { 0x1F6A50F0, 0x1F86A50F },
  { 0x1E86A50F, 0x1F86A50F },
  { 0x1E6A50F0, 0x1F86A50F },
  { 0x1D86A50F, 0x1F86A50F },
  { 0x1D6A50F0, 0x1F86A50F },
  { 0x1C86A50F, 0x1F86A50F },
  { 0x1C6A50F0, 0x1F86A50F },
  { 0x1B86A50F, 0x1F86A50F },
  { 0x1B6A50F0, 0x1F86A50F },
  { 0x1A86A50F, 0x1F86A50F },
  { 0x1A6A50F0, 0x1F86A50F },
  { 0x1986A50F, 0x1F86A50F },
  { 0x196A50F0, 0x1F86A50F },
  { 0x1886A50F, 0x1F86A50F },
  { 0x186A50F0, 0x1F86A50F },
  { 0x1786A50F, 0x1F86A50F },
  { 0x176A50F0, 0x1F86A50F },
  { 0x1686A50F, 0x1F86A50F },
  { 0x166A50F0, 0x1F86A50F },
  { 0x1586A50F, 0x1F86A50F },
  { 0x156A50F0, 0x1F86A50F },
  { 0x1486A50F, 0x1F86A50F },
  { 0x146A50F0, 0x1F86A50F },
  { 0x1386A50F, 0x1F86A50F },
  { 0x136A50F0, 0x1F86A50F },
  { 0x1286A50F, 0x1F86A50F },
  { 0x126A50F0, 0x1F86A50F },
  { 0x1186A50F, 0x1F86A50F },
  { 0x1106A50F, 0x1F86A50F },
  /*
   * other tests
   */
  { 0x3FABBF00, 0x3FABC000 },
  { 0x403FFFDF, 0x7EFFFFB0 },
  { 0x007FFC00, 0x3F800400 },
  { 0x00100000, 0x40FFFFFF },
  { 0x817FE003, 0x3E801000 },
  { 0x80080100, 0xC17FE003 },
};

int numPairs = sizeof(pairs)/sizeof(pairs[0]);


void check_selected(void) {
  _FP_Word x, y;
  Bool z, r;
  int i;
  Bool error;

  error = false;
  for (i = 0; i < numPairs; i++) {
    x = pairs[i].x;
    y = pairs[i].y;
    Flags = 0;
    z = fpCmp(x, y);
    Flags_ref = 0;
    r = fpCmp_ref(x, y);
    showBool("r", r);
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


#define SKIP_MASK       0x001FFFFC


void check_intervals(void) {
  _FP_Word x, y;
  Bool z, r;
  long count, errors;

  printf("------------------------------------------------\n");
  printf("Part 1: full small interval [0x3FABBBBB, 0x3FABCDEF]\n");
  count = 0;
  errors = 0;
  x = 0x3FABBBBB;
  do {
    y = 0x3FABBBBB;
    do {
      Flags = 0;
      z = fpCmp(x, y);
      Flags_ref = 0;
      r = fpCmp_ref(x, y);
      count++;
      if (z != r || Flags != Flags_ref) {
        if (errors == 0) {
          printf("first error: x = 0x%08X, y = 0x%08X\n", x, y);
          printf("             z = 0x%08X, r = 0x%08X\n", z, r);
        }
        errors++;
      }
      y++;
    } while (y <= 0x3FABCDEF);
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
    y = 0x3F000000;
    do {
      Flags = 0;
      z = fpCmp(x, y);
      Flags_ref = 0;
      r = fpCmp_ref(x, y);
      count++;
      if (z != r || Flags != Flags_ref) {
        if (errors == 0) {
          printf("first error: x = 0x%08X, y = 0x%08X\n", x, y);
          printf("             z = 0x%08X, r = 0x%08X\n", z, r);
        }
        errors++;
      }
      y += 2999;
    } while (y <= 0x40000000);
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
    y = 0;
    do {
      if ((y & SKIP_MASK) != 0x00000000 &&
          (y & SKIP_MASK) != SKIP_MASK) {
        y |= SKIP_MASK;
      }
      Flags = 0;
      z = fpCmp(x, y);
      Flags_ref = 0;
      r = fpCmp_ref(x, y);
      count++;
      if (z != r || Flags != Flags_ref) {
        if (errors == 0) {
          printf("first error: x = 0x%08X, y = 0x%08X\n", x, y);
          printf("             z = 0x%08X, r = 0x%08X\n", z, r);
        }
        errors++;
      }
      y++;
    } while (y != 0);
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


void server(void) {
  char line[LINE_SIZE];
  char *endptr;
  _FP_Word x, y;
  Bool z;

  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    if (line[0] == '#') continue;
    endptr = line;
    x = strtoul(endptr, &endptr, 16);
    y = strtoul(endptr, &endptr, 16);
    Flags = 0;
    z = fpCmp(x, y);
    printf("%08X %08X %d %02X\n", x, y, z, Flags);
  }
}


/**************************************************************/

/*
 * main program
 */


void usage(char *myself) {
  printf("usage: %s\n"
         "       (-eq | -ne | -le | -lt)\n"
         "       (-simple | -selected | -intervals | -server)\n",
         myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc != 3) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-eq") == 0) {
    predicate = PRED_EQ;
  } else
  if (strcmp(argv[1], "-ne") == 0) {
    predicate = PRED_NE;
  } else
  if (strcmp(argv[1], "-le") == 0) {
    predicate = PRED_LE;
  } else
  if (strcmp(argv[1], "-lt") == 0) {
    predicate = PRED_LT;
  } else {
    usage(argv[0]);
  }
  if (strcmp(argv[2], "-simple") == 0) {
    startDevice(PATH_TO_FPSERVER);
    printf("Check simple test cases\n");
    check_simple();
    stopDevice();
  } else
  if (strcmp(argv[2], "-selected") == 0) {
    startDevice(PATH_TO_FPSERVER);
    printf("Check selected test cases\n");
    check_selected();
    stopDevice();
  } else
  if (strcmp(argv[2], "-intervals") == 0) {
    startDevice(PATH_TO_FPSERVER);
    printf("Check different intervals\n");
    check_intervals();
    stopDevice();
  } else
  if (strcmp(argv[2], "-server") == 0) {
    startDevice(PATH_TO_FPSERVER);
    server();
    stopDevice();
  } else {
    usage(argv[0]);
  }
  return 0;
}
