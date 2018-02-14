#include "romlib.h"


/*
 * Count the number of characters needed to represent
 * a given number in base 10.
 */
static int countPrintn(long n) {
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
static void *printn(void *(*emit)(void *, char), void *arg,
                    int *nchar, long n) {
  long a;

  if (n < 0) {
    arg = emit(arg, '-');
    (*nchar)++;
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    arg = printn(emit, arg, nchar, a);
  }
  arg = emit(arg, n % 10 + '0');
  (*nchar)++;
  return arg;
}


/*
 * Count the number of characters needed to represent
 * a given number in a given base.
 */
static int countPrintu(unsigned long n, unsigned long b) {
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
static void *printu(void *(*emit)(void *, char), void *arg,
                    int *nchar, unsigned long n, unsigned long b,
                    Bool upperCase) {
  unsigned long a;

  a = n / b;
  if (a != 0) {
    arg = printu(emit, arg, nchar, a, b, upperCase);
  }
  if (upperCase) {
    arg = emit(arg, "0123456789ABCDEF"[n % b]);
    (*nchar)++;
  } else {
    arg = emit(arg, "0123456789abcdef"[n % b]);
    (*nchar)++;
  }
  return arg;
}


/*
 * Output a number of filler characters.
 */
static void *fill(void *(*emit)(void *, char), void *arg,
                  int *nchar, int numFillers, char filler) {
  while (numFillers-- > 0) {
    arg = emit(arg, filler);
    (*nchar)++;
  }
  return arg;
}


/*
 * This function does the real work of formatted printing.
 */
static int doPrintf(void *(*emit)(void *, char), void *arg,
                    const char *fmt, va_list ap) {
  int nchar;
  char c;
  int n;
  long ln;
  unsigned int u;
  unsigned long lu;
  char *s;
  Bool negFlag;
  char filler;
  int width, count;

  nchar = 0;
  while (1) {
    while ((c = *fmt++) != '%') {
      if (c == '\0') {
        return nchar;
      }
      arg = emit(arg, c);
      nchar++;
    }
    c = *fmt++;
    if (c == '-') {
      negFlag = true;
      c = *fmt++;
    } else {
      negFlag = false;
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
        arg = fill(emit, arg, &nchar, width - count, filler);
      }
      arg = printn(emit, arg, &nchar, n);
      if (width > 0 && negFlag) {
        arg = fill(emit, arg, &nchar, width - count, filler);
      }
    } else
    if (c == 'u' || c == 'o' || c == 'x' || c == 'X') {
      u = va_arg(ap, int);
      count = countPrintu(u,
                c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10));
      if (width > 0 && !negFlag) {
        arg = fill(emit, arg, &nchar, width - count, filler);
      }
      arg = printu(emit, arg, &nchar, u,
                   c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10),
                   c == 'X');
      if (width > 0 && negFlag) {
        arg = fill(emit, arg, &nchar, width - count, filler);
      }
    } else
    if (c == 'l') {
      c = *fmt++;
      if (c == 'd') {
        ln = va_arg(ap, long);
        count = countPrintn(ln);
        if (width > 0 && !negFlag) {
          arg = fill(emit, arg, &nchar, width - count, filler);
        }
        arg = printn(emit, arg, &nchar, ln);
        if (width > 0 && negFlag) {
          arg = fill(emit, arg, &nchar, width - count, filler);
        }
      } else
      if (c == 'u' || c == 'o' || c == 'x' || c == 'X') {
        lu = va_arg(ap, long);
        count = countPrintu(lu,
                  c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10));
        if (width > 0 && !negFlag) {
          arg = fill(emit, arg, &nchar, width - count, filler);
        }
        arg = printu(emit, arg, &nchar, lu,
                     c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10),
                     c == 'X');
        if (width > 0 && negFlag) {
          arg = fill(emit, arg, &nchar, width - count, filler);
        }
      } else {
        arg = emit(arg, 'l');
        nchar++;
        arg = emit(arg, c);
        nchar++;
      }
    } else
    if (c == 's') {
      s = va_arg(ap, char *);
      count = strlen(s);
      if (width > 0 && !negFlag) {
        arg = fill(emit, arg, &nchar, width - count, filler);
      }
      while ((c = *s++) != '\0') {
        arg = emit(arg, c);
        nchar++;
      }
      if (width > 0 && negFlag) {
        arg = fill(emit, arg, &nchar, width - count, filler);
      }
    } else
    if (c == 'c') {
      c = va_arg(ap, char);
      arg = emit(arg, c);
      nchar++;
    } else {
      arg = emit(arg, c);
      nchar++;
    }
  }
  /* never reached */
  return 0;
}


/*
 * Emit a character to the console.
 */
static void *emitToConsole(void *dummy, char c) {
  putchar(c);
  return dummy;
}


/*
 * Formatted output with a variable argument list.
 */
int vprintf(const char *fmt, va_list ap) {
  int n;

  n = doPrintf(emitToConsole, NULL, fmt, ap);
  return n;
}


/*
 * Formatted output.
 */
int printf(const char *fmt, ...) {
  int n;
  va_list ap;

  va_start(ap, fmt);
  n = vprintf(fmt, ap);
  va_end(ap);
  return n;
}


/*
 * Emit a character to a buffer.
 */
static void *emitToBuffer(void *bufptr, char c) {
  *(char *)bufptr = c;
  return (char *) bufptr + 1;
}


/*
 * Formatted output into a buffer with a variable argument list.
 */
int vsprintf(char *s, const char *fmt, va_list ap) {
  int n;

  n = doPrintf(emitToBuffer, s, fmt, ap);
  s[n] = '\0';
  return n;
}


/*
 * Formatted output into a buffer.
 */
int sprintf(char *s, const char *fmt, ...) {
  int n;
  va_list ap;

  va_start(ap, fmt);
  n = vsprintf(s, fmt, ap);
  va_end(ap);
  return n;
}
