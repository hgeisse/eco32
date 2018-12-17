/*
 * dctest.c -- compute correct answers for demo dcache test
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* #define DEBUG */

#define INDEX(i, j)	((i) * 16 + (j))


/**************************************************************/


#define MEMSIZE		(1 << 14)
#define MEMMASK		(MEMSIZE - 1)


unsigned int mem[MEMSIZE];


unsigned int readLine(unsigned int addr) {
  return mem[addr & MEMMASK];
}


void writeLine(unsigned int addr, unsigned int line) {
  mem[addr & MEMMASK] = line;
}


/**************************************************************/


unsigned char readByte(unsigned int addr) {
  unsigned int line;
  unsigned char data;

  line = readLine(addr >> 2);
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
#ifdef DEBUG
  printf("R 0x%04x 0x%02x\n", addr, data);
#endif
  return data;
}


void writeByte(unsigned int addr, unsigned char data) {
  unsigned int line;

#ifdef DEBUG
  printf("W 0x%04x 0x%02x\n", addr, data);
#endif
  line = readLine(addr >> 2);
  switch (addr & 3) {
    case 0:
      line &= ~(0xFF << 24);
      line |= ((unsigned int) data << 24);
      break;
    case 1:
      line &= ~(0xFF << 16);
      line |= ((unsigned int) data << 16);
      break;
    case 2:
      line &= ~(0xFF <<  8);
      line |= ((unsigned int) data <<  8);
      break;
    case 3:
      line &= ~(0xFF <<  0);
      line |= ((unsigned int) data <<  0);
      break;
  }
  writeLine(addr >> 2, line);
}


/**************************************************************/


unsigned int map(unsigned int addr) {
  if (addr >= 0x0100) {
    printf("Error: address out of range\n");
    exit(1);
  }
  if (addr < 0x0020) {
    addr += 0x1160;
  } else
  if (addr >= 0x0020 && addr < 0x0040) {
    addr += 0x2140;
  } else
  if (addr >= 0x0040 && addr < 0x0060) {
    addr += 0x3120;
  } else
  if (addr >= 0x0060 && addr < 0x0080) {
    addr += 0x4100;
  } else
  if (addr >= 0x0080 && addr < 0x00A0) {
    addr += 0x5100;
  } else
  if (addr >= 0x00A0 && addr < 0x00C0) {
    addr += 0x61E0;
  } else
  if (addr >= 0x00C0 && addr < 0x00E0) {
    addr += 0x7100;
  } else
  if (addr >= 0x00E0) {
    addr += 0x81E0;
  }
  return addr;
}


int main(int argc, char *argv[]) {
  unsigned int addr;
  unsigned char data;
  unsigned char sum;
  int i, j;
  int n, m;

  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      addr = map(INDEX(i, j));
      data = INDEX(i, j);
      writeByte(addr, data);
    }
  }
  for (i = 1; i <= 14; i++) {
    for (j = 1; j <= 14; j++) {
      sum = 0;
      for (n = 0; n < 3; n++) {
        for (m = 0; m < 3; m++) {
          addr = map(INDEX(i - 1 + n, j - 1 + m));
          data = readByte(addr);
          sum ^= data;
        }
      }
      addr = map(INDEX(i, j));
      data = sum;
      writeByte(addr, data);
    }
  }
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      addr = map(INDEX(i, j));
      data = readByte(addr);
    }
  }
#ifndef DEBUG
  printf("data matrix at end of test:\n");
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      addr = map(INDEX(i, j));
      data = readByte(addr);
      printf("0x%02x ", data);
    }
    printf("\n");
  }
#endif
  return 0;
}
