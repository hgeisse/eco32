/*
 * stdio.h -- input and output
 */


#ifndef _STDIO_H_
#define _STDIO_H_


#ifndef NULL
#define NULL		((void*)0)
#endif

#define FILENAME_MAX	20
#define L_tmpnam	FILENAME_MAX


#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
typedef unsigned long size_t;
#endif

#ifndef _VA_LIST_DEFINED_
#define _VA_LIST_DEFINED_
typedef char *va_list;
#endif

typedef struct {
  int dummy;
} FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


FILE *fopen(const char *filename, const char *mode);
FILE *freopen(const char *filename, const char *mode, FILE *stream);
int fflush(FILE *stream);
int fclose(FILE *stream);
int remove(const char *filename);
int rename(const char *oldname, const char *newname);
FILE *tmpfile(void);
char *tmpnam(char s[L_tmpnam]);
int setvbuf(FILE *stream, char *buf, int mode, size_t size);
void setbuf(FILE *stream, char *buf);

int fprintf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);
int sprintf(char *s, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list arg);
int vprintf(const char *format, va_list arg);
int vsprintf(char *s, const char *format, va_list arg);

void perror(const char *s);


#endif /* _STDIO_H_ */
