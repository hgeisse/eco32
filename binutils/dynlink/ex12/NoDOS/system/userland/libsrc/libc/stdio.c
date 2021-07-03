/*
 * stdio.c -- standard I/O library
 */


#include "stdio.h"
#include "string.h"
#include "syscalls.h"


typedef int Bool;

#define FALSE		0
#define TRUE		1


/**************************************************************/

/* file I/O */


FILE *fopen(char *path) {
  return _fopen(path);
}


void fclose(FILE *fp) {
  _fclose(fp);
}


int fseek(FILE *fp, int offset, int whence) {
  return _fseek(fp, offset, whence);
}


int ftell(FILE *fp) {
  return _ftell(fp);
}


int fread(void *buf, int size, int num, FILE *fp) {
  return _fread(buf, size, num, fp);
}


/**************************************************************/

/* terminal I/O */


char getchar(void) {
  return _getchar();
}


void putchar(char c) {
  if (c == '\n') {
    _putchar('\r');
  }
  _putchar(c);
}


void putString(char *s) {
  while (*s != '\0') {
    putchar(*s++);
  }
}


/**************************************************************/

/* get a line from the terminal */


void getLine(char *prompt, char *line, int max) {
  int index;
  char c;

  putString(prompt);
  index = 0;
  while (1) {
    c = getchar();
    switch (c) {
      case '\r':
        putchar('\n');
        line[index] = '\0';
        return;
      case '\b':
      case 0x7F:
        if (index == 0) {
          break;
        }
        putchar('\b');
        putchar(' ');
        putchar('\b');
        index--;
        break;
      default:
        if (c == '\t') {
          c = ' ';
        }
        if (c < 0x20 || c > 0x7E) {
          break;
        }
        putchar(c);
        line[index++] = c;
        break;
    }
  }
}


/**************************************************************/

/* scaled-down version of printf */


/*
 * Count the number of characters needed to represent
 * a given number in base 10.
 */
int countPrintn(long n) {
  long a;
  int res;

  res = 0;
  if (n < 0) {
    res++;
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    res += countPrintn(a);
  }
  return res + 1;
}


/*
 * Output a number in base 10.
 */
void printn(long n) {
  long a;

  if (n < 0) {
    putchar('-');
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    printn(a);
  }
  putchar(n % 10 + '0');
}


/*
 * Count the number of characters needed to represent
 * a given number in a given base.
 */
int countPrintu(unsigned long n, unsigned long b) {
  unsigned long a;
  int res;

  res = 0;
  a = n / b;
  if (a != 0) {
    res += countPrintu(a, b);
  }
  return res + 1;
}


/*
 * Output a number in a given base.
 */
void printu(unsigned long n, unsigned long b, Bool upperCase) {
  unsigned long a;

  a = n / b;
  if (a != 0) {
    printu(a, b, upperCase);
  }
  if (upperCase) {
    putchar("0123456789ABCDEF"[n % b]);
  } else {
    putchar("0123456789abcdef"[n % b]);
  }
}


/*
 * Output a number of filler characters.
 */
void fill(int numFillers, char filler) {
  while (numFillers-- > 0) {
    putchar(filler);
  }
}


/*
 * Formatted output with a variable argument list.
 */
void vprintf(char *fmt, va_list ap) {
  char c;
  int n;
  long ln;
  unsigned int u;
  unsigned long lu;
  char *s;
  Bool negFlag;
  char filler;
  int width, count;

  while (1) {
    while ((c = *fmt++) != '%') {
      if (c == '\0') {
        return;
      }
      putchar(c);
    }
    c = *fmt++;
    if (c == '-') {
      negFlag = TRUE;
      c = *fmt++;
    } else {
      negFlag = FALSE;
    }
    if (c == '0') {
      filler = '0';
      c = *fmt++;
    } else {
      filler = ' ';
    }
    width = 0;
    while (c >= '0' && c <= '9') {
      width *= 10;
      width += c - '0';
      c = *fmt++;
    }
    if (c == 'd') {
      n = va_arg(ap, int);
      count = countPrintn(n);
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      printn(n);
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'u' || c == 'o' || c == 'x' || c == 'X') {
      u = va_arg(ap, int);
      count = countPrintu(u,
                c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10));
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      printu(u,
             c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10),
             c == 'X');
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'l') {
      c = *fmt++;
      if (c == 'd') {
        ln = va_arg(ap, long);
        count = countPrintn(ln);
        if (width > 0 && !negFlag) {
          fill(width - count, filler);
        }
        printn(ln);
        if (width > 0 && negFlag) {
          fill(width - count, filler);
        }
      } else
      if (c == 'u' || c == 'o' || c == 'x' || c == 'X') {
        lu = va_arg(ap, long);
        count = countPrintu(lu,
                  c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10));
        if (width > 0 && !negFlag) {
          fill(width - count, filler);
        }
        printu(lu,
               c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10),
               c == 'X');
        if (width > 0 && negFlag) {
          fill(width - count, filler);
        }
      } else {
        putchar('l');
        putchar(c);
      }
    } else
    if (c == 's') {
      s = va_arg(ap, char *);
      count = strlen(s);
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      while ((c = *s++) != '\0') {
        putchar(c);
      }
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'c') {
      c = va_arg(ap, char);
      putchar(c);
    } else {
      putchar(c);
    }
  }
}


/*
 * Formatted output.
 * This is a scaled-down version of the C library's
 * printf. Used to print diagnostic information on
 * the console (and optionally to a logfile).
 */
void printf(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}
