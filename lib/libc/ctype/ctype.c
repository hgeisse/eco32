/*
 * ctype.c -- character class tests
 */


#include "ctype.h"


#define _D	(_S | _C)
#define _E	(_S | _B)
#define _M	(_N | _X)
#define _Y	(_U | _X)
#define _Z	(_L | _X)


unsigned char _ctype[] = {
  /*   -1 */   0,
  /* 0x00 */  _C, _C, _C, _C, _C, _C, _C, _C, _C, _D, _D, _D, _D, _D, _C, _C,
  /* 0x10 */  _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, _C,
  /* 0x20 */  _E, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P,
  /* 0x30 */  _M, _M, _M, _M, _M, _M, _M, _M, _M, _M, _P, _P, _P, _P, _P, _P,
  /* 0x40 */  _P, _Y, _Y, _Y, _Y, _Y, _Y, _U, _U, _U, _U, _U, _U, _U, _U, _U,
  /* 0x50 */  _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _P, _P, _P, _P, _P,
  /* 0x60 */  _P, _Z, _Z, _Z, _Z, _Z, _Z, _L, _L, _L, _L, _L, _L, _L, _L, _L,
  /* 0x70 */  _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _P, _P, _P, _P, _C,
  /* 0x80 */   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 0x90 */   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 0xA0 */   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 0xB0 */   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 0xC0 */   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 0xD0 */   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 0xE0 */   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 0xF0 */   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};


int tolower(int c) {
  return (isupper(c)) ? (c | 0x20) : c;
}


int toupper(int c) {
  return (islower(c)) ? (c & ~0x20) : c;
}
