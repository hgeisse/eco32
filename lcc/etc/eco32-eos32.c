/*
 * Code for ECO32 under EOS32
 * University of Applied Sciences Giessen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *suffixes[] = {
  /* 0 */  ".c",	/* C source files */
  /* 1 */  ".i",	/* preprocessed source files */
  /* 2 */  ".s",	/* assembly language files */
  /* 3 */  ".o",	/* object files */
  /* 4 */  ".out",	/* executable files */
  0
};

char inputs[256] = "";

char *include[] = {
  "-I" "/usr/include",
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
  "-target=eco32/eos32",
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
  "-L" "/usr/lib",
  "-o", "$3",		/* linker output file (executable) */
  "$1",			/* other options handed through */
  "/usr/lib/crt0.o",
  "$2",			/* linker input files (object) */
  "-lc",
  "", "",		/* reserved for "-s <ldscript>" */
  "", "",		/* reserved for "-m <ldmap>" */
  0
};

extern char *concat(char *, char *);

/*
 * We recognize the following additional options:
 *
 *   -Wo-nostdinc	do not search the standard dirs for header files
 *   -Wo-nostdlib	do not use the standard libs and startup files
 *   -Wo-ldscript=...	specify linker script file name
 *   -Wo-ldmap=...	specify linker map file name
 */
int option(char *arg) {
  if (strcmp(arg, "-nostdinc") == 0) {
    include[0] = NULL;
    return 1;
  }
  if (strcmp(arg, "-nostdlib") == 0) {
    ld[1] = "";
    ld[5] = "";
    ld[7] = "";
    return 1;
  }
  if (strncmp(arg, "-ldscript=", 10) == 0) {
    ld[8] = "-s";
    ld[9] = arg + 10;
    return 1;
  }
  if (strncmp(arg, "-ldmap=", 7) == 0) {
    ld[10] = "-m";
    ld[11] = arg + 7;
    return 1;
  }
  return 0;
}
