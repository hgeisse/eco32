#include "romlib.h"


/*
 * Count the length of a string (without terminating null character).
 */
int strlen(const char *s) {
  const char *p;

  p = s;
  while (*p != '\0') {
    p++;
  }
  return p - s;
}


/*
 * Compare two strings.
 * Return a number < 0, = 0, or > 0 iff the first string is less
 * than, equal to, or greater than the second one, respectively.
 */
int strcmp(const char *s, const char *t) {
  while (*s == *t) {
    if (*s == '\0') {
      return 0;
    }
    s++;
    t++;
  }
  return *s - *t;
}


/*
 * Copy string t to string s (includes terminating null character).
 */
char *strcpy(char *s, const char *t) {
  char *p;

  p = s;
  while ((*p = *t) != '\0') {
    p++;
    t++;
  }
  return s;
}


/*
 * Append string t to string s.
 */
char *strcat(char *s, const char *t) {
  char *p;

  p = s;
  while (*p != '\0') {
    p++;
  }
  while ((*p = *t) != '\0') {
    p++;
    t++;
  }
  return s;
}


/*
 * Locate character c in string s.
 */
char *strchr(const char *s, char c) {
  while (*s != c) {
    if (*s == '\0') {
      return NULL;
    }
    s++;
  }
  return (char *) s;
}


/*
 * Extract the next token from the string s, delimited
 * by any character from the delimiter string t.
 */
char *strtok(char *s, const char *t) {
  static char *p;
  char *q;

  if (s != NULL) {
    p = s;
  } else {
    p++;
  }
  while (*p != '\0' && strchr(t, *p) != NULL) {
    p++;
  }
  if (*p == '\0') {
    return NULL;
  }
  q = p++;
  while (*p != '\0' && strchr(t, *p) == NULL) {
    p++;
  }
  if (*p != '\0') {
    *p = '\0';
  } else {
    p--;
  }
  return q;
}
