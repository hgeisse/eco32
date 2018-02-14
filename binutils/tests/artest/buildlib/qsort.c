#include "romlib.h"


/*
 * Exchange two array items of a given size.
 */
static void xchg(char *p, char *q, int size) {
  char t;

  while (size--) {
    t = *p;
    *p++ = *q;
    *q++ = t;
  }
}


/*
 * This is a recursive version of quicksort.
 */
static void sort(char *l, char *r, int size,
                 int (*cmp)(const void *, const void *)) {
  char *i;
  char *j;
  char *x;

  i = l;
  j = r;
  x = l + (((r - l) / size) / 2) * size;
  do {
    while (cmp(i, x) < 0) {
      i += size;
    }
    while (cmp(x, j) < 0) {
      j -= size;
    }
    if (i <= j) {
      /* exchange array elements i and j */
      /* attention: update x if it is one of these */
      if (x == i) {
        x = j;
      } else
      if (x == j) {
        x = i;
      }
      xchg(i, j, size);
      i += size;
      j -= size;
    }
  } while (i <= j);
  if (l < j) {
    sort(l, j, size, cmp);
  }
  if (i < r) {
    sort(i, r, size, cmp);
  }
}


/*
 * External interface for the quicksort algorithm.
 */
void qsort(void *base, int n, int size,
           int (*cmp)(const void *, const void*)) {
  sort((char *) base, (char *) base + (n - 1) * size, size, cmp);
}
