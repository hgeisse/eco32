/*
 * crc7.c -- compute 7-bit CRC for SD cards
 */


/*
 * Examples:
 *
 * CMD0  (Arg=0) --> 01 000000 00000000000000000000000000000000 "1001010" 1
 * CMD17 (Arg=0) --> 01 010001 00000000000000000000000000000000 "0101010" 1
 * Resp of CMD17 --> 00 010001 00000000000000000000100100000000 "0110011" 1
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *example[] = {
  "0100000000000000000000000000000000000000",
  "0101000100000000000000000000000000000000",
  "0001000100000000000000000000100100000000",
  "0100100000000000000000000000000110101010",
  "0111011100000000000000000000000000000000",
  "0110100101000000000000000000000000000000",
  "0111101000000000000000000000000000000000",
  "0101000100000000000000000000011111000010",
  "0111101100000000000000000000000000000000",
  "0111101100000000000000000000000000000001",
};

int nexamples = sizeof(example) / sizeof(example[0]);


unsigned char crc_1(char *bits) {
  unsigned char res;
  unsigned char b, c;
  int i;

  res = 0;
  for (i = 0; i < strlen(bits); i++) {
    if (bits[i] == '0') {
      b = 0;
    } else
    if (bits[i] == '1') {
      b = 1;
    } else {
      printf("illegal character in bit string\n");
    }
    c = (res ^ b) & 1;
    res >>= 1;
    if (c) {
      res ^= 0x48;
    }
  }
  return res;
}


unsigned char crc_2(char *bits) {
  unsigned char res;
  unsigned char b, c;
  int i;

  res = 0;
  for (i = 0; i < strlen(bits); i++) {
    if (bits[i] == '0') {
      b = 0;
    } else
    if (bits[i] == '1') {
      b = 1;
    } else {
      printf("illegal character in bit string\n");
    }
    c = ((res >> 6) & 1) ^ b;
    res = (res << 1) & 0x7F;
    if (c) {
      res ^= 0x09;
    }
  }
  return res;
}


int main(int argc, char *argv[]) {
  int i, j;
  unsigned char chk;

  printf("\n");
  printf("Method 1\n");
  printf("--------\n");
  for (i = 0; i < nexamples; i++) {
    chk = crc_1(example[i]);
    printf("bit string %s has CRC value ", example[i]);
    for (j = 0; j < 7; j++) {
      printf("%c", chk & (1 << j) ? '1' : '0');
    }
    printf("_1\n");
  }
  printf("\n");
  printf("Method 2\n");
  printf("--------\n");
  for (i = 0; i < nexamples; i++) {
    chk = crc_2(example[i]);
    printf("bit string %s has CRC value ", example[i]);
    for (j = 6; j >= 0; j--) {
      printf("%c", chk & (1 << j) ? '1' : '0');
    }
    printf("_1\n");
  }
  printf("\n");
  return 0;
}
