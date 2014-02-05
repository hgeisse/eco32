/*
 * auxlib.h -- auxiliary library for standalone programs
 */


#ifndef _AUXLIB_H_
#define _AUXLIB_H_


int strlen(char *str);
int strcmp(char *str1, char *str2);
void strcpy(char *dst, char *src);

char getchar(void);
void putchar(char c);

void getLine(char *prompt, char *line, int max);

void vprintf(char *fmt, va_list ap);
void printf(char *fmt, ...);


#endif /* _AUXLIB_H_ */
