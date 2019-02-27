/*
 * h2x.c -- Hauser-to-hex converter
 */


#include <stdio.h>
#include <stdlib.h>


void usage(char *myself) {
  printf("usage: %s <fp number in Hauser's notation>\n", myself);
  printf("       example: +85.76E979\n");
  printf("       sign and dot are required\n");
  printf("       hex digits may be lower case\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  char *p;
  unsigned int sign;
  unsigned int exp;
  unsigned int frac;
  unsigned int flt;
  char *endp;

  if (argc != 2) {
    usage(argv[0]);
  }
  p = argv[1];
  if (*p == '+') {
    sign = 0x00000000;
  } else
  if (*p == '-') {
    sign = 0x80000000;
  } else {
    usage(argv[0]);
  }
  p++;
  exp = strtoul(p, &endp, 16);
  if (*endp != '.') {
    usage(argv[0]);
  }
  if (exp > 0x000000FF) {
    printf("exponent 0x%08X too big\n", exp);
    exit(1);
  }
  p = endp + 1;
  frac = strtoul(p, &endp, 16);
  if (*endp != '\0') {
    usage(argv[0]);
  }
  if (frac > 0x007FFFFF) {
    printf("fraction 0x%08X too big\n", frac);
    exit(1);
  }
  flt = sign | (exp << 23) | frac;
  printf("[%s] = 0x%08X = %e\n",
         argv[1], flt, *(float *)&flt);
  return 0;
}
