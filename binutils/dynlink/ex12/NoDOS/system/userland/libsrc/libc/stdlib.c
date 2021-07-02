/*
 * stdlib.c -- standard library
 */


#include "stdlib.h"


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
