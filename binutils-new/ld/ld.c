/*
 * ld.c -- ECO32 linking loader
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../include/a.out.h"


/**************************************************************/


#define MAX_INFILES	100


/**************************************************************/


char *inNames[MAX_INFILES];
int numInfiles;


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}


/**************************************************************/


void usage(char *myself) {
  fprintf(stderr, "Usage: %s\n", myself);
  fprintf(stderr, "         [-o objfile]     set output file name\n");
  fprintf(stderr, "         file             object file name\n");
  fprintf(stderr, "         [files...]       additional obj/lib files\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  char *argp;
  char *outName;
  FILE *outFile;

  numInfiles = 0;
  outName = "a.out";
  for (i = 1; i < argc; i++) {
    argp = argv[i];
    if (*argp == '-') {
      /* option */
      argp++;
      switch (*argp) {
        case 'o':
          if (i == argc - 1) {
            usage(argv[0]);
          }
          outName = argv[++i];
          break;
        default:
          usage(argv[0]);
      }
    } else {
      /* file */
      if (numInfiles == MAX_INFILES) {
        error("too many input files");
      }
      inNames[numInfiles++] = argp;
    }
  }
  if (numInfiles == 0) {
    error("no input files");
  }
  outFile = fopen(outName, "w");
  if (outFile == NULL) {
    error("cannot open output file '%s'", outName);
  }
  fclose(outFile);
  return 0;
}
