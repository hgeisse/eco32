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
static FILE *progImage;
static unsigned int progSize;


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


void romWrite(Word pAddr, Word *src, int nWords) {
  /* throw bus timeout exception */
  throwException(EXC_BUS_TIMEOUT);
}


void romReset(void) {
  unsigned int i;

  cPrintf("Resetting ROM...\n");
  for (i = 0; i < ROM_SIZE; i++) {
    rom[i] = 0xFF;
  }
  if (progImage != NULL) {
    fseek(progImage, 0, SEEK_SET);
    if (fread(rom, progSize, 1, progImage) != 1) {
      error("cannot read ROM image file");
    }
    cPrintf("%6d KB ROM installed, %d bytes programmed.\n",
            ROM_SIZE / K, progSize);
  }
}


void romInit(char *progImageName) {
  /* allocate ROM */
  rom = malloc(ROM_SIZE);
  if (rom == NULL) {
    error("cannot allocate ROM");
  }
  /* possibly load ROM image */
  if (progImageName == NULL) {
    /* no ROM to plug in */
    progImage = NULL;
  } else {
    /* plug in ROM */
    progImage = fopen(progImageName, "rb");
    if (progImage == NULL) {
      error("cannot open ROM image '%s'", progImageName);
    }
    fseek(progImage, 0, SEEK_END);
    progSize = ftell(progImage);
    if (progSize > ROM_SIZE) {
      error("ROM image too big");
    }
    /* do actual loading of image in romReset() */
  }
  romReset();
}


void romExit(void) {
  free(rom);
}
