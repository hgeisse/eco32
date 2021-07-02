/*
 * stdio.h -- standard I/O library
 */


#ifndef _STDIO_H_
#define _STDIO_H_


#include "stdarg.h"


#ifndef NULL
#define NULL	((void *) 0)
#endif


#ifndef BLOCK_SIZE
#define BLOCK_SIZE		4096
#endif


#define SEEK_SET		0
#define SEEK_CUR		1
#define SEEK_END		2


typedef struct {
  int slotInUse;
  int blockStart;
  int byteSize;
  int filePos;
  int bufBlkno;
  unsigned char buf[BLOCK_SIZE];
} FILE;


FILE *fopen(char *path);
void fclose(FILE *fp);
int fseek(FILE *fp, int offset, int whence);
int ftell(FILE *fp);
int fread(void *buf, int size, int num, FILE *fp);

char getchar(void);
void putchar(char c);
void putString(char *s);
void getLine(char *prompt, char *line, int max);
void vprintf(char *fmt, va_list ap);
void printf(char *fmt, ...);


#endif /* _STDIO_H_ */
