/*
 * patch.c -- file patcher
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
  FILE *file;
  unsigned int offset;
  unsigned char value;

  file = fopen(argv[1], "r+");
  if (file == NULL) {
    printf("cannot open file\n");
    return 1;
  }
  offset = strtoul(argv[2], NULL, 0);
  value = strtoul(argv[3], NULL, 0);
  fseek(file, offset, SEEK_SET);
  if (fwrite(&value, 1, 1, file) != 1) {
    printf("cannot write file\n");
    return 1;
  }
  fclose(file);
  return 0;
}
