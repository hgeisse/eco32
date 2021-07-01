/*
 * random.c - generate a file with 16 * 4 KB random numbers
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


unsigned int randomNumber = 0xDEADBEEF;


unsigned int getRandomNumber(void) {
  randomNumber = randomNumber * (unsigned) 1103515245 + (unsigned) 12345;
  return randomNumber;
}


int main(int argc, char *argv[]) {
  FILE *out;
  int i;
  unsigned int n;
  unsigned char m[4];

  if (argc != 2) {
    printf("usage: %s <output file>\n", argv[0]);
    exit(1);
  }
  out = fopen(argv[1], "w");
  if (out == NULL) {
    printf("cannot open output file\n");
    exit(1);
  }
  for (i = 0; i < (16 * 4096) / 4; i++) {
    n = getRandomNumber();
    m[0] = (n >> 24) & 0xFF;
    m[1] = (n >> 16) & 0xFF;
    m[2] = (n >>  8) & 0xFF;
    m[3] = (n >>  0) & 0xFF;
    fwrite(m, 1, 4, out);
  }
  fclose(out);
  return 0;
}
