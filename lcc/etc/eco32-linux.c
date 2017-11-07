/*
 * Code for ECO32 under Linux
 * University of Applied Sciences Giessen
 */

#include <string.h>

char *suffixes[] = {
  /* 0 */  ".c",	/* C source files */
  /* 1 */  ".i",	/* preprocessed source files */
  /* 2 */  ".s",	/* assembly language files */
  /* 3 */  ".o",	/* object files */
  /* 4 */  ".out",	/* ??? */
  0
};

char inputs[256] = "";

char *include[] = {
  "-I" LCCDIR "../include",
  0
};

char *cpp[] = {
  LCCDIR "cpp",
  "-D__STDC__=1",
  "-Dunix",
  "-Deco32",
  "$1",			/* preprocessor include directories */
  "$2",			/* preprocessor input file (C) */
  "$3",			/* preprocessor output file (preprocessed C) */
  0
};

char *com[] = {
  LCCDIR "rcc",
  "-target=eco32/linux",
  "$1",			/* other options handed through */
  "$2",			/* compiler input file (preprocessed C) */
  "$3",			/* compiler output file (assembler) */
  0
};

char *as[] = {
  LCCDIR "as",
  "-o", "$3",		/* assembler output file (object) */
  "$1",			/* other options handed through */
  "$2",			/* assembler input file (assembler) */
  0
};

char *ld[] = {
  LCCDIR "ld",
  "-L" LCCDIR "../lib",
  "-o", "$3",		/* linker output file (executable) */
  "$1",			/* other options handed through */
  LCCDIR "../lib/crt0.o",
  "$2",			/* linker input files (object) */
  "-lc",
  0
};

extern char *concat(char *, char *);

int option(char *arg) {
  return 0;
}
