/*
 * wrboot.c -- write the boot block
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


/**************************************************************/


#define SSIZE		512	/* disk sector size in bytes */
#define BSIZE		8192	/* disk block size in bytes */


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
}


/**************************************************************/


unsigned int read4FromEco(unsigned char *p) {
  return (unsigned int) p[0] << 24 |
         (unsigned int) p[1] << 16 |
         (unsigned int) p[2] <<  8 |
         (unsigned int) p[3] <<  0;
}


/**************************************************************/


void usage(char *myself) {
  printf("Usage: %s <bootblock> <disk> <partition or '*'>\n", myself);
}


int main(int argc, char *argv[]) {
  int i;
  char *bootName;
  FILE *bootFile;
  int bootSize;
  char *diskName;
  FILE *diskFile;
  unsigned int sliceStart;
  unsigned int sliceSize;
  int part;
  char *endptr;
  unsigned char partTable[SSIZE];
  unsigned char *ptptr;
  unsigned int partType;
  unsigned char bootCode[BSIZE];
  unsigned char diskLabel[SSIZE];

  if (argc != 4) {
    usage(argv[0]);
    exit(1);
  }
  /* read bootblock from file */
  for (i = 0; i < BSIZE; i++) {
    bootCode[i] = 0;
  }
  bootName = argv[1];
  bootFile = fopen(bootName, "rb");
  if (bootFile == NULL) {
    error("cannot open bootblock '%s'", bootName);
  }
  fseek(bootFile, 0, SEEK_END);
  bootSize = ftell(bootFile);
  fseek(bootFile, 0, SEEK_SET);
  if (bootSize > BSIZE) {
    error("bootblock '%s' too big", bootName);
  }
  if (fread(bootCode, 1, bootSize, bootFile) != bootSize) {
    error("cannot read bootblock '%s'", bootName);
  }
  fclose(bootFile);
  printf("Bootblock '%s' read, %d bytes.\n", bootName, bootSize);
  /* determine NetBSD slice position on disk */
  diskName = argv[2];
  diskFile = fopen(diskName, "r+b");
  if (diskFile == NULL) {
    error("cannot open disk image '%s'", diskName);
  }
  if (strcmp(argv[3], "*") == 0) {
    /* whole disk contains one single slice */
    sliceStart = 0;
    fseek(diskFile, 0, SEEK_END);
    sliceSize = ftell(diskFile) / SSIZE;
  } else {
    /* argv[3] is partition number of NetBSD slice */
    part = strtoul(argv[3], &endptr, 10);
    if (*endptr != '\0' || part < 0 || part > 15) {
      error("illegal partition number '%s'", argv[3]);
    }
    fseek(diskFile, 1 * SSIZE, SEEK_SET);
    if (fread(partTable, 1, SSIZE, diskFile) != SSIZE) {
      error("cannot read partition table of disk '%s'", diskName);
    }
    ptptr = partTable + part * 32;
    partType = read4FromEco(ptptr + 0);
    if ((partType & 0x7FFFFFFF) != 0x000000A9) {
      error("partition %d of disk '%s' is not a NetBSD slice",
            part, diskName);
    }
    sliceStart = read4FromEco(ptptr + 4);
    sliceSize = read4FromEco(ptptr + 8);
  }
  printf("Slice size is %u (0x%X) sectors of %d bytes each.\n",
         sliceSize, sliceSize, SSIZE);
  /* read disklabel from disk */
  fseek(diskFile, (sliceStart + 1) * SSIZE, SEEK_SET);
  if (fread(diskLabel, 1, SSIZE, diskFile) != SSIZE) {
    error("cannot read disklabel of disk '%s'", diskName);
  }
  printf("Disklabel read from disk '%s', %d bytes\n", diskName, SSIZE);
  /* copy disklabel to bootblock in memory */
  for (i = 0; i < SSIZE; i++) {
    bootCode[SSIZE + i] = diskLabel[i];
  }
  /* write bootblock to disk */
  fseek(diskFile, (sliceStart + 0) * SSIZE, SEEK_SET);
  if (fwrite(bootCode, 1, bootSize, diskFile) != bootSize) {
    error("cannot write bootblock to disk '%s'", diskName);
  }
  printf("Bootblock written to disk '%s', %d bytes\n", diskName, bootSize);
  fclose(diskFile);
  return 0;
}
