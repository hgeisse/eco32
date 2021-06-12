/*
 * common.h -- common definitions
 */


#ifndef _COMMON_H_
#define _COMMON_H_


typedef unsigned char Byte;		/* 8 bit quantities */
typedef unsigned short Half;		/* 16 bit quantities */
typedef unsigned int Word;		/* 32 bit quantities */


void error(char *fmt, ...);
void warning(char *fmt, ...);


#endif /* _COMMON_H_ */
