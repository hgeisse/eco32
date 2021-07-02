/*
 * string.h -- string library
 */


#ifndef _STRING_H_
#define _STRING_H_


#ifndef NULL
#define NULL	((void *) 0)
#endif


int strlen(char *str);
int strcmp(char *s1, char *s2);
void strcpy(char *dst, char *src);
void strcat(char *dst, char *src);
char *strchr(const char *cs, int c);
char *strtok(char *str, char *delim);
void memcpy(unsigned char *dst, unsigned char *src, unsigned int cnt);
void memset(unsigned char *dst, unsigned char c, unsigned int cnt);


#endif /* _STRING_H_ */
