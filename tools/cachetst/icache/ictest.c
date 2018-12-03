/*
 * ictest.c -- compute correct answers for demo icache test
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NUM_TESTS	1000


/**************************************************************/


unsigned int memory(unsigned int addr) {
  unsigned int a_01_00;
  unsigned int a_05_02;
  unsigned int a_09_06;
  unsigned int a_13_10;

  a_01_00 = (addr >>  0) & 0x03;
  a_05_02 = (addr >>  2) & 0x0F;
  a_09_06 = (addr >>  6) & 0x0F;
  a_13_10 = (addr >> 10) & 0x0F;
  return ((~a_01_00 & 0x03) << 30) |
         (( a_05_02 & 0x0F) << 26) |
         (( 0       & 0x03) << 24) |
         ((~a_09_06 & 0x0F) << 20) |
         (( a_13_10 & 0x0F) << 16) |
         (( a_09_06 & 0x0F) << 12) |
         ((~0       & 0x03) << 10) |
         ((~a_13_10 & 0x0F) <<  6) |
         (( a_01_00 & 0x03) <<  4) |
         ((~a_05_02 & 0x0F) <<  0);
}


/**************************************************************/


unsigned int nextAddr(void) {
  static unsigned int state = 0;	/* 20-bit generator state */
  static unsigned int naddr = 0;	/* 16-bit next address */
  unsigned int addr;

  addr = naddr;
  switch (state) {
    case 0x0003F:
      naddr = 0x0000;
      break;
    case 0x0007F:
      naddr = 0x0000;
      break;
    case 0x000BF:
      naddr = 0x0000;
      break;
    case 0x000FF:
      naddr = 0x8000;
      break;
    case 0x0013F:
      naddr = 0x8000;
      break;
    case 0x0017F:
      naddr = 0x8000;
      break;
    case 0x001BF:
      naddr = 0x8000;
      break;
    case 0x001FF:
      naddr = 0x0000;
      break;
    default:
      naddr++;
      break;
  }
  state = (state + 1) & 0x000FFFFF;
  naddr &= 0x0000FFFF;
  return addr;
}


int main(int argc, char *argv[]) {
  int testCount;
  unsigned int addr;
  unsigned int line;
  unsigned char data;

  for (testCount = 0; testCount < NUM_TESTS; testCount++) {
    addr = nextAddr();
    line = memory(addr >> 2);
    switch (addr & 3) {
      case 0:
        data = (line >> 24) & 0xFF;
        break;
      case 1:
        data = (line >> 16) & 0xFF;
        break;
      case 2:
        data = (line >>  8) & 0xFF;
        break;
      case 3:
        data = (line >>  0) & 0xFF;
        break;
    }
    printf("test 0x%05X: addr = 0x%04X, line = 0x%08X, data = 0x%02X\n",
           testCount, addr, line, data);
  }
  return 0;
}
