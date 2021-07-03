/*
 * man.c -- print the description of a command to standard output
 */


#include "stdio.h"
#include "string.h"


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
  char path[100];

  if (argc < 2) {
    printf("missing command name\n");
    return 1;
  }
  strcpy(path, "/man/");
  strcat(path, argv[1]);
  return cat(path);
}
