/*
 * string.h -- string functions
 */


#ifndef _STRING_H_
#define _STRING_H_


#ifndef NULL
#define NULL		((void*)0)
#endif


#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
typedef unsigned long size_t;
#endif


char *strcpy(char *s, const char *ct);
char *strncpy(char *s, const char *ct, size_t n);
char *strcat(char *s, const char *ct);
char *strncat(char *s, const char *ct, size_t n);
int strcmp(const char *cs, const char *ct);
int strncmp(const char *cs, const char *ct, size_t n);
char *strchr(const char *cs, int c);
char *strrchr(const char *cs, int c);
size_t strspn(const char *cs, const char *ct);
size_t strcspn(const char *cs, const char *ct);
char *strpbrk(const char *cs, const char *ct);
char *strstr(const char *cs, const char *ct);
size_t strlen(const char *cs);
char *strerror(int n);
char *strtok(char *s, const char *ct);

void *memcpy(void *s, const void *ct, size_t n);
void *memmove(void *s, const void *ct, size_t n);
int memcmp(const void *cs, const void *ct, size_t n);
void *memchr(const void *cs, int c, size_t n);
void *memset(void *s, int c, size_t n);


#endif /* _STRING_H_ */
