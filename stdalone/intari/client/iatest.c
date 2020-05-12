/*
 * iatest.c -- integer arithmetic test
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>


#define OP_ADD		1
#define OP_SUB		2
#define OP_MULU		3
#define OP_DIVU		4
#define OP_REMU		5
#define OP_MULI		6
#define OP_DIVI		7
#define OP_REMI		8
#define OP_EQ		9
#define OP_NE		10
#define OP_LEU		11
#define OP_LTU		12
#define OP_GEU		13
#define OP_GTU		14
#define OP_LEI		15
#define OP_LTI		16
#define OP_GEI		17
#define OP_GTI		18

#define SYN		((Byte) 's')
#define ACK		((Byte) 'a')


typedef unsigned int Word;
typedef unsigned char Byte;


/**************************************************************/


Word patterns[] = {
  0x00000000,
  0x00000001,
  0x00000002,
  0x00000003,
  0x0000000E,
  0x3FFFFFFF,
  0x40000000,
  0x40000001,
  0x7FFFFFFE,
  0x7FFFFFFF,
  0x80000000,
  0x80000001,
  0x80000002,
  0xBFFFFFFF,
  0xC0000000,
  0xC0000001,
  0xFFFFFFF2,
  0xFFFFFFFD,
  0xFFFFFFFE,
  0xFFFFFFFF,
};

int n = sizeof(patterns) / sizeof(patterns[0]);


FILE *outFile;
int sfd = 0;


/**************************************************************/


void serialClose(void);


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  serialClose();
  exit(1);
}


/**************************************************************/


struct termios origOptions;
struct termios currOptions;


void serialOpen(char *serialPort) {
  sfd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);
  if (sfd == -1) {
    error("cannot open serial port '%s'", serialPort);
  }
  tcgetattr(sfd, &origOptions);
  currOptions = origOptions;
  cfsetispeed(&currOptions, B38400);
  cfsetospeed(&currOptions, B38400);
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
  if (sfd == 0) {
    return;
  }
  tcsetattr(sfd, TCSANOW, &origOptions);
  close(sfd);
  sfd = 0;
}


int serialSnd(Byte b) {
  int n;

  n = write(sfd, &b, 1);
  return n == 1;
}


int serialRcv(Byte *bp) {
  int n;

  n = read(sfd, bp, 1);
  return n == 1;
}


void connect(void) {
  Byte b;

  while (!serialSnd(SYN)) ;
  tcdrain(sfd);
  while (!serialRcv(&b)) ;
  if (b != ACK) {
    error("cannot synchronize with client");
  }
}


void serialSendByte(Byte b) {
  while (!serialSnd(b)) ;
}


void serialSendWord(Word w) {
  serialSendByte((w >> 24) & 0xFF);
  serialSendByte((w >> 16) & 0xFF);
  serialSendByte((w >>  8) & 0xFF);
  serialSendByte((w >>  0) & 0xFF);
}


void serialRecvByte(Byte *bp) {
  Byte b;

  while (!serialRcv(&b)) ;
  *bp = b;
}


void serialReceiveWord(Word *wp) {
  Byte b3, b2, b1, b0;

  serialRecvByte(&b3);
  serialRecvByte(&b2);
  serialRecvByte(&b1);
  serialRecvByte(&b0);
  *wp = ((Word) b3 << 24) |
        ((Word) b2 << 16) |
        ((Word) b1 <<  8) |
        ((Word) b0 <<  0);
}


/**************************************************************/


Word solveProblemIntern(char op, Word x, Word y) {
  Word answer;

  switch (op) {
    case OP_ADD:
      answer = x + y;
      break;
    case OP_SUB:
      answer = x - y;
      break;
    case OP_MULU:
      answer = x * y;
      break;
    case OP_DIVU:
      answer = x / y;
      break;
    case OP_REMU:
      answer = x % y;
      break;
    case OP_MULI:
      answer = (int) x * (int) y;
      break;
    case OP_DIVI:
      if (x == 0x80000000 && y == 0xFFFFFFFF) {
        answer = 0;
      } else {
        answer = (int) x / (int) y;
      }
      break;
    case OP_REMI:
      if (x == 0x80000000 && y == 0xFFFFFFFF) {
        answer = 0;
      } else {
        answer = (int) x % (int) y;
      }
      break;
    case OP_EQ:
      answer = (x == y);
      break;
    case OP_NE:
      answer = (x != y);
      break;
    case OP_LEU:
      answer = (x <= y);
      break;
    case OP_LTU:
      answer = (x < y);
      break;
    case OP_GEU:
      answer = (x >= y);
      break;
    case OP_GTU:
      answer = (x > y);
      break;
    case OP_LEI:
      answer = ((int) x <= (int) y);
      break;
    case OP_LTI:
      answer = ((int) x < (int) y);
      break;
    case OP_GEI:
      answer = ((int) x >= (int) y);
      break;
    case OP_GTI:
      answer = ((int) x > (int) y);
      break;
    default:
      error("unknown operator %d in solveProblemToIntern()", (int) op);
      break;
  }
  return answer;
}


/**************************************************************/


Word solveProblem(char op, Word x, Word y) {
  Word r;

  if (sfd == 0) {
    return solveProblemIntern(op, x, y);
  }
  serialSendByte(op);
  serialSendWord(x);
  serialSendWord(y);
  serialReceiveWord(&r);
  return r;
}


/**************************************************************/


void add(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_ADD, x, y);
      fprintf(outFile, "0x%08X + 0x%08X = 0x%08X\n", x, y, r);
    }
  }
}


void sub(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_SUB, x, y);
      fprintf(outFile, "0x%08X - 0x%08X = 0x%08X\n", x, y, r);
    }
  }
}


void mulu(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_MULU, x, y);
      fprintf(outFile, "0x%08X *u 0x%08X = 0x%08X\n", x, y, r);
    }
  }
}


void divu(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      if (y == 0) continue;
      r = solveProblem(OP_DIVU, x, y);
      fprintf(outFile, "0x%08X /u 0x%08X = 0x%08X\n", x, y, r);
    }
  }
}


void remu(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      if (y == 0) continue;
      r = solveProblem(OP_REMU, x, y);
      fprintf(outFile, "0x%08X %%u 0x%08X = 0x%08X\n", x, y, r);
    }
  }
}


void muli(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_MULI, x, y);
      fprintf(outFile, "0x%08X * 0x%08X = 0x%08X", x, y, r);
      fprintf(outFile, " | ");
      fprintf(outFile, "%d * %d = %d\n", (int) x, (int) y, (int) r);
    }
  }
}


void divi(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      if (y == 0) continue;
      r = solveProblem(OP_DIVI, x, y);
      fprintf(outFile, "0x%08X / 0x%08X = 0x%08X", x, y, r);
      fprintf(outFile, " | ");
      fprintf(outFile, "%d / %d = %d\n", (int) x, (int) y, (int) r);
    }
  }
}


void remi(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      if (y == 0) continue;
      r = solveProblem(OP_REMI, x, y);
      fprintf(outFile, "0x%08X %% 0x%08X = 0x%08X", x, y, r);
      fprintf(outFile, " | ");
      fprintf(outFile, "%d %% %d = %d\n", (int) x, (int) y, (int) r);
    }
  }
}


void eq(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_EQ, x, y);
      fprintf(outFile, "0x%08X == 0x%08X : %c\n", x, y, r ? 'y' : 'n');
    }
  }
}


void ne(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_NE, x, y);
      fprintf(outFile, "0x%08X != 0x%08X : %c\n", x, y, r ? 'y' : 'n');
    }
  }
}


void leu(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_LEU, x, y);
      fprintf(outFile, "0x%08X <=u 0x%08X : %c\n", x, y, r ? 'y' : 'n');
    }
  }
}


void ltu(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_LTU, x, y);
      fprintf(outFile, "0x%08X <u 0x%08X : %c\n", x, y, r ? 'y' : 'n');
    }
  }
}


void geu(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_GEU, x, y);
      fprintf(outFile, "0x%08X >=u 0x%08X : %c\n", x, y, r ? 'y' : 'n');
    }
  }
}


void gtu(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_GTU, x, y);
      fprintf(outFile, "0x%08X >u 0x%08X : %c\n", x, y, r ? 'y' : 'n');
    }
  }
}


void lei(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_LEI, x, y);
      fprintf(outFile, "0x%08X <= 0x%08X : %c", x, y, r ? 'y' : 'n');
      fprintf(outFile, " | ");
      fprintf(outFile, "%d <= %d : %c\n", (int) x, (int) y, r ? 'y' : 'n');
    }
  }
}


void lti(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_LTI, x, y);
      fprintf(outFile, "0x%08X < 0x%08X : %c", x, y, r ? 'y' : 'n');
      fprintf(outFile, " | ");
      fprintf(outFile, "%d < %d : %c\n", (int) x, (int) y, r ? 'y' : 'n');
    }
  }
}


void gei(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_GEI, x, y);
      fprintf(outFile, "0x%08X >= 0x%08X : %c", x, y, r ? 'y' : 'n');
      fprintf(outFile, " | ");
      fprintf(outFile, "%d >= %d : %c\n", (int) x, (int) y, r ? 'y' : 'n');
    }
  }
}


void gti(void) {
  int i, j;
  Word x, y, r;

  for (i = 0; i < n; i++) {
    x = patterns[i];
    for (j = 0; j < n; j++) {
      y = patterns[j];
      r = solveProblem(OP_GTI, x, y);
      fprintf(outFile, "0x%08X > 0x%08X : %c", x, y, r ? 'y' : 'n');
      fprintf(outFile, " | ");
      fprintf(outFile, "%d > %d : %c\n", (int) x, (int) y, r ? 'y' : 'n');
    }
  }
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s <output file> [<serial line>]\n", myself);
  printf("  if the serial line is omitted, results ");
  printf("are computed internally\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  char *outName;
  char *serName;

  if (argc != 2 && argc != 3) {
    usage(argv[0]);
  }
  outName = argv[1];
  serName = NULL;
  if (argc == 3) {
    serName = argv[2];
  }
  outFile = fopen(outName, "w");
  if (outFile == NULL) {
    error("cannot open output file '%s'", outName);
  }
  if (serName != NULL) {
    serialOpen(serName);
    printf("Trying to connect...\n");
    connect();
    printf("...connected\n");
  }
  printf("testing add\n");
  add();
  printf("testing sub\n");
  sub();
  printf("testing mulu\n");
  mulu();
  printf("testing divu\n");
  divu();
  printf("testing remu\n");
  remu();
  printf("testing muli\n");
  muli();
  printf("testing divi\n");
  divi();
  printf("testing remi\n");
  remi();
  printf("testing eq\n");
  eq();
  printf("testing ne\n");
  ne();
  printf("testing leu\n");
  leu();
  printf("testing ltu\n");
  ltu();
  printf("testing geu\n");
  geu();
  printf("testing gtu\n");
  gtu();
  printf("testing lei\n");
  lei();
  printf("testing lti\n");
  lti();
  printf("testing gei\n");
  gei();
  printf("testing gti\n");
  gti();
  fclose(outFile);
  if (serName != NULL) {
    serialClose();
  }
  return 0;
}
