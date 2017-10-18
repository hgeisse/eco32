/*
 * utils.h -- utility functions
 */


#ifndef _UTILS_H_
#define _UTILS_H_


void error(char *fmt, ...);
void warning(char *fmt, ...);
void *memAlloc(unsigned int size);
void memFree(void *p);


#endif /* _UTILS_H_ */
