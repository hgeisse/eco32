/*
 * iolib.c -- I/O library
 */


#include "types.h"
#include "stdarg.h"
#include "iolib.h"
#include "biolib.h"


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


int strcmp(char *s1, char *s2) {
  while (*s1 == *s2) {
    if (*s1 == '\0') {
      return 0;
    }
    s1++;
    s2++;
  }
  return *s1 - *s2;
}


void strcpy(char *dst, char *src) {
  while ((*dst++ = *src++) != '\0') ;
}


void strcat(char *dst, char *src) {
  while (*dst++ != '\0') ;
  dst--;
  while ((*dst++ = *src++) != '\0') ;
}


char *strchr(const char *cs, int c) {
  do {
    if (*cs == c) {
      return (char *) cs;
    }
  } while (*cs++ != '\0');
  return NULL;
}


char *strtok(char *s, char *ct) {
  static char *p;
  char *q;

  if (s != NULL) {
    p = s;
  } else {
    p++;
  }
  while (*p != '\0' && strchr(ct, *p) != NULL) {
    p++;
  }
  if (*p == '\0') {
    return NULL;
  }
  q = p++;
  while (*p != '\0' && strchr(ct, *p) == NULL) {
    p++;
  }
  if (*p != '\0') {
    *p = '\0';
  } else {
    p--;
  }
  return q;
}


void memcpy(unsigned char *dst, unsigned char *src, unsigned int cnt) {
  while (cnt--) {
    *dst++ = *src++;
  }
}


void memset(unsigned char *dst, unsigned char c, unsigned int cnt) {
  while (cnt--) {
    *dst++ = c;
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


/**************************************************************/

/*
 * memory allocation
 *
 * Allocation is done in differently sized blocks, which
 * comprise a header and the usable memory proper. That
 * consists itself of an integral number of header-sized
 * 'units'.
 *
 * Each block, allocated or free, begins with a header.
 * All free blocks are linked on a free list.
 */


#define MAX_TOP		0xC0100000
#define MIN_ALLOC	1024	/* in header-sized units */


typedef struct header {
  struct header *next;		/* next block if on free list */
  unsigned int size;		/* size of this block in units */
				/* (including its own header) */
} Header;


extern unsigned char _end;	/* after loading: top of .bss */
static unsigned char *top;	/* top address of current heap */
static Header base;		/* empty list to get started */
static Header *freep = NULL;	/* points to start of free list */


static Header *moreCore(unsigned int nu) {
  unsigned char *newTop;
  Header *up;

  if (nu < MIN_ALLOC) {
    nu = MIN_ALLOC;
  }
  newTop = top + nu * sizeof(Header);
  if ((unsigned int) newTop > (unsigned int) MAX_TOP) {
    return NULL;
  }
  up = (Header *) top;
  top = newTop;
  up->size = nu;
  free((void *) (up + 1));
  return freep;
}


void *malloc(unsigned int size) {
  unsigned int nunits;
  Header *prevp;
  Header *p;

  nunits = (size + sizeof(Header) - 1) / sizeof(Header) + 1;
  if (freep == NULL) {
    /* no free list yet */
    top = &_end;
    base.next = &base;
    base.size = 0;
    freep = &base;
  }
  prevp = freep;
  p = prevp->next;
  while (1) {
    if (p->size >= nunits) {
      /* big enough */
      if (p->size == nunits) {
        /* exact fit */
        prevp->next = p->next;
      } else {
        /* allocate tail end */
        p->size -= nunits;
        p += p->size;
        p->size = nunits;
      }
      freep = prevp;
      return (void *) (p + 1);
    }
    if (p == freep) {
      /* wrapped around free list */
      p = moreCore(nunits);
      if (p == NULL) {
        /* no memory left */
        return NULL;
      }
    }
    /* advance pointers */
    prevp = p;
    p = p->next;
  }
  /* never reached */
  return NULL;
}


void free(void *p) {
  Header *hp;
  Header *q;

  /* if p is NULL there is nothing to be freed */
  if (p == NULL) {
    return;
  }
  /* let hp point to block header */
  hp = (Header *) p - 1;
  /* find right place in free list */
  for (q = freep; !(hp > q && hp < q->next); q = q->next) {
    if (q >= q->next && (hp > q || hp < q->next)) {
      /* freed block at start or end of arena */
      break;
    }
  }
  /* check for joining neighbours */
  if (hp + hp->size == q->next) {
    /* join to upper neighbour */
    hp->size += q->next->size;
    hp->next = q->next->next;
  } else {
    hp->next = q->next;
  }
  if (q + q->size == hp) {
    /* join to lower neighbour */
    q->size += hp->size;
    q->next = hp->next;
  } else {
    q->next = hp;
  }
  freep = q;
}
