/*
 * argret.c -- print command-line arguments, return argc
 */


#include "stdio.h"


int main(int argc, char *argv[]) {
  int i;

  for (i = 0; i < argc; i++) {
    printf("%d: '%s'\n", i, argv[i]);
  }
  return argc;
}
