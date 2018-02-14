#include "romlib.h"


extern char cin(void);
extern void cout(char);


/*
 * Input a character from the console.
 */
char getchar(void) {
  return cin();
}


/*
 * Output a character on the console.
 * Replace LF by CR/LF.
 */
void putchar(char c) {
  if (c == '\n') {
    cout('\r');
  }
  cout(c);
}


/*
 * Output a string on the console.
 * Replace LF by CR/LF.
 */
void puts(const char *s) {
  while (*s != '\0') {
    putchar(*s);
    s++;
  }
}
