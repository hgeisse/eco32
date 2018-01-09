/*
 * rom.c -- ROM simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "rom.h"


static Byte *rom;
static unsigned int romSize;
static FILE *romImage;


void romRead(Word pAddr, Word *dst, int nWords) {
  Word data;

  while (nWords--) {
    if (pAddr > ROM_BASE + ROM_SIZE - 4) {
      /* throw bus timeout exception */
      throwException(EXC_BUS_TIMEOUT);
    }
    data = ((Word) *(rom + (pAddr - ROM_BASE) + 0)) << 24 |
           ((Word) *(rom + (pAddr - ROM_BASE) + 1)) << 16 |
           ((Word) *(rom + (pAddr - ROM_BASE) + 2)) <<  8 |
           ((Word) *(rom + (pAddr - ROM_BASE) + 3)) <<  0;
    *dst++ = data;
    pAddr += 4;
  }
}


void romReset(void) {
  unsigned int i;

  cPrintf("Resetting ROM...\n");
  for (i = 0; i < ROM_SIZE; i++) {
    rom[i] = 0xFF;
  }
  if (romImage != NULL) {
    fseek(romImage, 0, SEEK_SET);
    if (fread(rom, romSize, 1, romImage) != 1) {
      error("cannot read ROM image file");
    }
    cPrintf("%6d KB ROM installed, %d bytes programmed.\n",
            ROM_SIZE / K, romSize);
  }
}


void romInit(char *romImageName) {
  /* allocate ROM */
  rom = malloc(ROM_SIZE);
  if (rom == NULL) {
    error("cannot allocate ROM");
  }
  /* possibly load ROM image */
  if (romImageName == NULL) {
    /* no ROM to plug in */
    romImage = NULL;
  } else {
    /* plug in ROM */
    romImage = fopen(romImageName, "rb");
    if (romImage == NULL) {
      error("cannot open ROM image '%s'", romImageName);
    }
    fseek(romImage, 0, SEEK_END);
    romSize = ftell(romImage);
    if (romSize > ROM_SIZE) {
      error("ROM image too big");
    }
    /* do actual loading of image in romReset() */
  }
  romReset();
}


void romExit(void) {
  free(rom);
}
