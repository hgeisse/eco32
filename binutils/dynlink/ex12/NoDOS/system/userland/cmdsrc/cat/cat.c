/*
 * cat.c -- print the contents of a file to standard output
 */


#include "stdio.h"


int cat(char *path) {
  FILE *fp;
  char c;

  fp = fopen(path);
  if (fp == NULL) {
    printf("cannot open file '%s'\n", path);
    return 1;
  }
  while (fread(&c, 1, 1, fp) == 1) {
    putchar(c);
  }
  fclose(fp);
  return 0;
}


int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("missing file name\n");
    return 1;
  }
  return cat(argv[1]);
}
