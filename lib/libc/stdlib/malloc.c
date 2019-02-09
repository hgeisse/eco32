/*
 * malloc.c -- dynamic memory allocation
 */


#include "stdlib.h"
#include "sys/syscall.h"


#define MIN_ALLOC	1024	/* minimum number of units to request */


typedef long Align;

typedef union header {
  struct {
    union header *ptr;		/* next block if on free list */
    unsigned int size;		/* size of this block in header units */
  } s;
  Align x;			/* force alignment */
} Header;


static Header base;
static Header *freep;


static Header *moreCore(unsigned int numUnits) {
  void *cp;

  if (numUnits < MIN_ALLOC) {
    numUnits = MIN_ALLOC;
  }
  cp = sbrk(numUnits * sizeof(Header));
  if (cp == (void *) (unsigned int) -1) {
    /* no space */
    return NULL;
  }
}


void *calloc(size_t nobj, size_t size) {
  size_t s;
  void *p;
  char *q;

  s = nobj * size;
  if (s <= 0) {
    return NULL;
  }
  p = malloc(s);
  if (p == NULL) {
    return NULL;
  }
  q = (char *) p;
  while (s-- != 0) {
    *q++ = '\0';
  }
  return p;
}


void *malloc(size_t size) {
  if (size <= 0) {
    return NULL;
  }
}


void *realloc(void *p, size_t size) {
}


void free(void *p) {
}
