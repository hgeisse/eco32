/*
 * chkdsk.c -- check disk file reads
 */


#include "stdio.h"


#define NUM_TESTS	20
#define MAX_WHERE	(12 * 4096)
#define MAX_COUNT	(4 * 4096)


unsigned char ref[] = {
  #include "../../../../tools/random/randbytes.txt"
};


unsigned int randomNumber = 11111111;


unsigned int getRandomNumber(void) {
  randomNumber = randomNumber * (unsigned) 1103515245 + (unsigned) 12345;
  return randomNumber;
}


int check(unsigned int where, unsigned int count, FILE *fp) {
  unsigned char buf[MAX_COUNT];
  int i;

  printf("reading %d bytes from offset %d\n", count, where);
  if (fseek(fp, where, SEEK_SET) < 0) {
    printf("cannot set position in file '/data/randbytes'\n");
    return 1;
  }
  if (fread(buf, 1, count, fp) != count) {
    printf("cannot read file '/data/randbytes'\n");
    return 1;
  }
  for (i = 0; i < count; i++) {
    if (buf[i] != ref[where + i]) {
      printf("data read not equal to reference\n");
      return 1;
    }
  }
  return 0;
}


int main(int argc, char *argv[]) {
  FILE *fp;
  int i;
  unsigned int where, count;

  fp = fopen("/data/randbytes");
  if (fp == NULL) {
    printf("cannot open file '/data/randbytes'\n");
    return 1;
  }
  if (check(0, 4095, fp)) {
    fclose(fp);
    return 1;
  }
  if (check(0, 4096, fp)) {
    fclose(fp);
    return 1;
  }
  if (check(0, 4097, fp)) {
    fclose(fp);
    return 1;
  }
  if (check(4 * 4096 + 4095, 4095, fp)) {
    fclose(fp);
    return 1;
  }
  if (check(4 * 4096 + 4096, 4096, fp)) {
    fclose(fp);
    return 1;
  }
  if (check(4 * 4096 + 4097, 4097, fp)) {
    fclose(fp);
    return 1;
  }
  if (check(7 * 4096 + 4095, 2 * 4096 + 4095, fp)) {
    fclose(fp);
    return 1;
  }
  if (check(7 * 4096 + 4096, 2 * 4096 + 4096, fp)) {
    fclose(fp);
    return 1;
  }
  if (check(7 * 4096 + 4097, 2 * 4096 + 4097, fp)) {
    fclose(fp);
    return 1;
  }
  for (i = 0; i < NUM_TESTS; i++) {
    where = (getRandomNumber() >> 5) % MAX_WHERE;
    do {
      count = (getRandomNumber() >> 5) % MAX_COUNT;
    } while (count == 0);
    if (check(where, count, fp)) {
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  printf("no errors\n");
  return 0;
}
