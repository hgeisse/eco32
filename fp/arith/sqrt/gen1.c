/*
 * gen1.c -- generate all test cases for a univariate function
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**************************************************************/


void generateAll(int skipSome) {
  unsigned int i;

  i = 0;
  do {
    if (skipSome) {
      while ((i & 0x0000FF00) != 0x00000000 &&
             (i & 0x0000FF00) != 0x0000FF00) {
        i += 0x00000100;
      }
    }
    printf("%08X\n", i);
    i++;
  } while (i != 0);
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s -most|-all\n", myself);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
    return 1;
  }
  if (strcmp(argv[1], "-most") == 0) {
    generateAll(1);
    return 0;
  }
  if (strcmp(argv[1], "-all") == 0) {
    generateAll(0);
    return 0;
  }
  usage(argv[0]);
  return 1;
}
