/*
 * hex-to-Hauser converter
 */


#include <stdio.h>
#include <stdlib.h>


void usage(char *myself) {
  printf("usage: %s <fp number in hex>\n", myself);
  printf("       example: 0xDEAD1234\n");
  printf("       prefix '0x' may be omitted\n");
  printf("       hex digits may be lower case\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  unsigned int flt;
  unsigned int sign;
  unsigned int exp;
  unsigned int frac;
  char *endp;

  if (argc != 2) {
    usage(argv[0]);
  }
  flt = strtoul(argv[1], &endp, 16);
  if (*endp != '\0') {
    usage(argv[0]);
  }
  sign = flt & 0x80000000;
  exp = (flt << 1) >> 24;
  frac = flt & 0x007FFFFF;
  printf("0x%08X = [%c%02X.%06X] = %e\n",
         flt, sign ? '-' : '+', exp, frac, *(float *)&flt);
  return 0;
}
