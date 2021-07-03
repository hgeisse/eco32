/*
 * syscalls.h -- system call interface
 */


#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_


#include "dirent.h"
#include "stdio.h"


void _halt(void);
char _getchar(void);
void _putchar(char c);

DIR *_opendir(void);
void _closedir(DIR *dp);
int _readdir(DirEntry *buf, DIR *dp);

FILE *_fopen(char *path);
void _fclose(FILE *fp);
int _fseek(FILE *fp, int offset, int whence);
int _ftell(FILE *fp);
int _fread(void *buf, int size, int num, FILE *fp);


#endif /* _SYSCALLS_H__ */
