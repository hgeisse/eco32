/*
 * iolib.h -- I/O library
 */


#ifndef _IOLIB_H_
#define _IOLIB_H_


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
char getchar(void);
void putchar(char c);
void putString(char *s);
void getLine(char *prompt, char *line, int max);
void vprintf(char *fmt, va_list ap);
void printf(char *fmt, ...);
void *malloc(unsigned int size);
void free(void *p);


#endif /* _IOLIB_H_ */
