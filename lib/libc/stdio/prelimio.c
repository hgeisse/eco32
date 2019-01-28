/*
 * types.h -- additional types
 */


#ifndef _TYPES_H_
#define _TYPES_H_


typedef int Bool;

#define FALSE	0
#define TRUE	1


#endif /* _TYPES_H_ */


/*
 * stdarg.h -- variable argument lists
 */


#ifndef _STDARG_H_
#define _STDARG_H_


static float __va_arg_tmp;


#define va_start(list, start) \
	((void)((list) = (sizeof(start)<4 ? \
	(char *)((int *)&(start)+1) : (char *)(&(start)+1))))

#define __va_arg(list, mode, n) \
	(__typecode(mode)==1 && sizeof(mode)==4 ? \
	(__va_arg_tmp = *(double *)(&(list += \
	((sizeof(double)+n)&~n))[-(int)((sizeof(double)+n)&~n)]), \
	*(mode *)&__va_arg_tmp) : \
	*(mode *)(&(list += \
	((sizeof(mode)+n)&~n))[-(int)((sizeof(mode)+n)&~n)]))

#define _bigendian_va_arg(list, mode, n) \
	(sizeof(mode)==1 ? *(mode *)(&(list += 4)[-1]) : \
	sizeof(mode)==2 ? *(mode *)(&(list += 4)[-2]) : \
	__va_arg(list, mode, n))

#define va_end(list) ((void)0)

#define va_arg(list, mode) \
	(sizeof(mode)==8 ? \
	*(mode *)(&(list = (char*)(((int)list + 15)&~7U))[-8]) : \
	_bigendian_va_arg(list, mode, 3U))


#endif /* _STDARG_H_ */


/*
 * biolib.h -- basic I/O library
 */


#ifndef _BIOLIB_H_
#define _BIOLIB_H_


char getc(void);
void putc(char c);


#endif /* _BIOLIB_H_ */


/*
 * iolib.h -- I/O library
 */


#ifndef _IOLIB_H_
#define _IOLIB_H_


int strlen(char *str);
void strcpy(char *dst, char *src);
void memcpy(unsigned char *dst, unsigned char *src, unsigned int cnt);
char getchar(void);
void putchar(char c);
void putString(char *s);
void getLine(char *prompt, char *line, int max);
int vprintf(const char *fmt, va_list ap);
int printf(const char *fmt, ...);


#endif /* _IOLIB_H_ */


/*
 * biolib.c -- basic I/O library
 */


char getc(void) {
  unsigned int *base;
  char c;

  base = (unsigned int *) 0xF0300000;
  while ((*(base + 0) & 1) == 0) ;
  c = *(base + 1);
  return c;
}


void putc(char c) {
  unsigned int *base;

  base = (unsigned int *) 0xF0300000;
  while ((*(base + 2) & 1) == 0) ;
  *(base + 3) = c;
}


/*
 * iolib.c -- I/O library
 */


#include "stdarg.h"


/**************************************************************/

/* string functions */


int strlen(char *str) {
  int i;

  i = 0;
  while (*str++ != '\0') {
    i++;
  }
  return i;
}


void strcpy(char *dst, char *src) {
  while ((*dst++ = *src++) != '\0') ;
}


void memcpy(unsigned char *dst, unsigned char *src, unsigned int cnt) {
  while (cnt--) {
    *dst++ = *src++;
  }
}


/**************************************************************/

/* terminal I/O */


char getchar(void) {
  return getc();
}


void putchar(char c) {
  if (c == '\n') {
    putchar('\r');
  }
  putc(c);
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
  putString(line);
  index = strlen(line);
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
int vprintf(const char *fmt, va_list ap) {
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
        return 0;
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
  return 0;
}


/*
 * Formatted output.
 * This is a scaled-down version of the C library's
 * printf. Used to print diagnostic information on
 * the console (and optionally to a logfile).
 */
int printf(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  return 0;
}
