/*
 * ctype.h -- character class tests
 */


#ifndef _CTYPE_H_
#define _CTYPE_H_


#define _U		0x80
#define _L		0x40
#define _N		0x20
#define _S		0x10
#define _P		0x08
#define _C		0x04
#define _B		0x02
#define _X		0x01

#define isalnum(c)	((_ctype + 1)[c] & (_U | _L | _N))
#define isalpha(c)	((_ctype + 1)[c] & (_U | _L))
#define iscntrl(c)	((_ctype + 1)[c] & (_C))
#define isdigit(c)	((_ctype + 1)[c] & (_N))
#define isgraph(c)	((_ctype + 1)[c] & (_U | _L | _N | _P))
#define islower(c)	((_ctype + 1)[c] & (_L))
#define isprint(c)	((_ctype + 1)[c] & (_U | _L | _N | _P | _B))
#define ispunct(c)	((_ctype + 1)[c] & (_P))
#define isspace(c)	((_ctype + 1)[c] & (_S))
#define isupper(c)	((_ctype + 1)[c] & (_U))
#define isxdigit(c)	((_ctype + 1)[c] & (_X))


extern unsigned char _ctype[];


int tolower(int c);
int toupper(int c);


#endif /* _CTYPE_H_ */
