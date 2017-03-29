/*
 * crc16.c -- compute 16-bit CRC for SD cards
 */


/*
 * Example:
 *
 * 512 bytes with 0xFF data --> CRC16 = 0x7FA1
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


unsigned short crc_1(void) {
  unsigned short res;
  unsigned short b, c;
  int i;

  res = 0;
  for (i = 0; i < 512 * 8; i++) {
    b = 1;
    c = (res ^ b) & 1;
    res >>= 1;
    if (c) {
      res ^= 0x8408;
    }
  }
  return res;
}


unsigned short crc_2(void) {
  unsigned short res;
  unsigned short b, c;
  int i;

  res = 0;
  for (i = 0; i < 512 * 8; i++) {
    b = 1;
    c = ((res >> 15) & 1) ^ b;
    res <<= 1;
    if (c) {
      res ^= 0x1021;
    }
  }
  return res;
}


int main(int argc, char *argv[]) {
  int j;
  unsigned short chk;

  printf("\n");
  printf("Method 1\n");
  printf("bit string of 512*8 '1' bits has CRC value ");
  chk = crc_1();
  for (j = 0; j < 16; j++) {
    printf("%c", chk & (1 << j) ? '1' : '0');
  }
  printf("\n");
  printf("\n");
  printf("Method 2\n");
  printf("bit string of 512*8 '1' bits has CRC value ");
  chk = crc_2();
  for (j = 15; j >= 0; j--) {
    printf("%c", chk & (1 << j) ? '1' : '0');
  }
  printf("\n");
  printf("\n");
  return 0;
}
