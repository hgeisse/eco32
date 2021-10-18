/*
 * flt.c -- implementation and test of conversion to float
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../include/fp.h"


#define SERIAL_PORT		"/dev/ttyUSB0"
#define LINE_SIZE		200


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
 * serial port
 */


#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>


int sfd = -1;
struct termios origOptions;
struct termios currOptions;


void serialClose(void);


void fatalError(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "FATAL ERROR: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  serialClose();
  exit(1);
}


void serialOpen(char *serialPort) {
  sfd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);
  if (sfd == -1) {
    fatalError("cannot open serial port '%s'", serialPort);
  }
  tcgetattr(sfd, &origOptions);
  currOptions = origOptions;
  cfsetispeed(&currOptions, B115200);
  cfsetospeed(&currOptions, B115200);
  currOptions.c_cflag |= (CLOCAL | CREAD);
  currOptions.c_cflag &= ~PARENB;
  currOptions.c_cflag &= ~CSTOPB;
  currOptions.c_cflag &= ~CSIZE;
  currOptions.c_cflag |= CS8;
  currOptions.c_cflag &= ~CRTSCTS;
  currOptions.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);
  currOptions.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK);
  currOptions.c_iflag &= ~(INPCK | ISTRIP | INLCR | IGNCR | ICRNL);
  currOptions.c_iflag &= ~(IXON | IXOFF | IXANY);
  currOptions.c_oflag &= ~(OPOST | ONLCR | OCRNL | ONOCR | ONLRET);
  tcsetattr(sfd, TCSANOW, &currOptions);
}


void serialClose(void) {
  if (sfd < 0) {
    return;
  }
  tcsetattr(sfd, TCSANOW, &origOptions);
  close(sfd);
  sfd = -1;
}


Bool serialSnd(unsigned char b) {
  int n;

  n = write(sfd, &b, 1);
  return n == 1;
}


Bool serialRcv(unsigned char *bp) {
  int n;

  n = read(sfd, bp, 1);
  return n == 1;
}


/**************************************************************/

/*
 * sending and receiving bytes and words
 */


void sndByte(unsigned char b) {
  while (!serialSnd(b)) ;
}


void sndWord(unsigned int w) {
  while (!serialSnd((w >>  0) & 0xFF)) ;
  while (!serialSnd((w >>  8) & 0xFF)) ;
  while (!serialSnd((w >> 16) & 0xFF)) ;
  while (!serialSnd((w >> 24) & 0xFF)) ;
}


unsigned char rcvByte(void) {
  unsigned char b;

  while (!serialRcv(&b)) ;
  return b;
}


unsigned int rcvWord(void) {
  unsigned int w;
  unsigned char b;

  w = 0;
  while (!serialRcv(&b)) ;
  w |= (unsigned int) b <<  0;
  while (!serialRcv(&b)) ;
  w |= (unsigned int) b <<  8;
  while (!serialRcv(&b)) ;
  w |= (unsigned int) b << 16;
  while (!serialRcv(&b)) ;
  w |= (unsigned int) b << 24;
  return w;
}


/**************************************************************/

/*
 * device communication
 */


void startDevice(char *serialPort) {
  serialOpen(serialPort);
}


void stopDevice(void) {
  serialClose();
}


void sndDevice(_FP_Word x) {
  sndWord(x);
}


void rcvDevice(_FP_Word *zp, _FP_Word *fp) {
  *zp = (_FP_Word) rcvWord();
  *fp = (_FP_Word) rcvByte();
}


/**************************************************************/

/*
 * floating-point float function, implementation
 */


_FP_Word Flags = 0;


_FP_Word fpFlt(int x) {
  _FP_Word z;

  sndDevice(x);
  rcvDevice(&z, &Flags);
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
  printf("usage: %s -simple | -selected | -intervals | -server\n",
         myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-simple") == 0) {
    startDevice(SERIAL_PORT);
    printf("Check simple test cases\n");
    check_simple();
    stopDevice();
  } else
  if (strcmp(argv[1], "-selected") == 0) {
    startDevice(SERIAL_PORT);
    printf("Check selected test cases\n");
    check_selected();
    stopDevice();
  } else
  if (strcmp(argv[1], "-intervals") == 0) {
    startDevice(SERIAL_PORT);
    printf("Check different intervals\n");
    check_intervals();
    stopDevice();
  } else
  if (strcmp(argv[1], "-server") == 0) {
    startDevice(SERIAL_PORT);
    server();
    stopDevice();
  } else {
    usage(argv[0]);
  }
  return 0;
}
