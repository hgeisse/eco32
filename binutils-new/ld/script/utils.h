/*
 * utils.h -- utility functions
 */


#ifndef _UTILS_H_
#define _UTILS_H_


void error(char *fmt, ...);
void warning(char *fmt, ...);

void *memAlloc(unsigned int size);
void memFree(void *p);

unsigned int read4FromEco(unsigned char *p);
void write4ToEco(unsigned char *p, unsigned int data);
void conv4FromEcoToNative(unsigned char *p);
void conv4FromNativeToEco(unsigned char *p);


#endif /* _UTILS_H_ */
