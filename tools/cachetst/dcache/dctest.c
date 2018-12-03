/*
 * dctest.c -- compute correct answers for demo dcache test
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
  return data;
}


void writeByte(unsigned int addr, unsigned char data) {
  unsigned int line;

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
  }
  if (addr < 0x0020) {
    addr += 0x1100;
  } else
  if (addr >= 0x0020 && addr < 0x0040) {
    addr += 0x2100;
  } else
  if (addr >= 0x0040 && addr < 0x0060) {
    addr += 0x3100;
  } else
  if (addr >= 0x0060 && addr < 0x0080) {
    addr += 0x4100;
  } else
  if (addr >= 0x0080 && addr < 0x00A0) {
    addr += 0x5100;
  } else
  if (addr >= 0x00A0 && addr < 0x00C0) {
    addr += 0x6100;
  } else
  if (addr >= 0x00C0 && addr < 0x00E0) {
    addr += 0x7100;
  } else
  if (addr >= 0x00E0) {
    addr += 0x8100;
  }
  return addr;
}


int main(int argc, char *argv[]) {
  unsigned int testCount;
  unsigned int addr;
  unsigned char data;
  unsigned char sum;
  int i, j;
  int n, m;

  testCount = 0;
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      addr = map(INDEX(i, j));
      data = INDEX(i, j);
      writeByte(addr, data);
      printf("test 0x%05X: acc = W, addr = 0x%04X, data = 0x%02X\n",
             testCount++, addr, data);
    }
  }
  for (i = 1; i <= 14; i++) {
    for (j = 1; j <= 14; j++) {
      sum = 0;
      for (n = 0; n < 3; n++) {
        for (m = 0; m < 3; m++) {
          addr = map(INDEX(i - 1 + n, j - 1 + m));
          data = readByte(addr);
          printf("test 0x%05X: acc = R, addr = 0x%04X, data = 0x%02X\n",
                 testCount++, addr, data);
          sum ^= data;
        }
      }
      addr = map(INDEX(i, j));
      data = sum;
      writeByte(addr, data);
      printf("test 0x%05X: acc = W, addr = 0x%04X, data = 0x%02X\n",
             testCount++, addr, data);
    }
  }
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      addr = map(INDEX(i, j));
      data = readByte(addr);
      printf("test 0x%05X: acc = R, addr = 0x%04X, data = 0x%02X\n",
             testCount++, addr, data);
    }
  }
  printf("\ndata matrix at end of test:\n");
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      addr = map(INDEX(i, j));
      data = readByte(addr);
      printf("0x%02X ", data);
    }
    printf("\n");
  }
  return 0;
}
