/*
 * stdlib.h -- standard library
 */


#ifndef _STDLIB_H_
#define _STDLIB_H_


#ifndef NULL
#define NULL	((void *) 0)
#endif


void *malloc(unsigned int size);
void free(void *p);


#endif /* _STDLIB_H_ */
