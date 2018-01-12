/*
 * ram.c -- RAM simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "ram.h"


static Byte *ram;
static unsigned int ramSize;
static FILE *progImage;
static unsigned int progSize;
static unsigned int progAddr;


void ramRead(Word pAddr, Word *dst, int nWords) {
  Word data;

  while (nWords--) {
    if (pAddr > RAM_BASE + ramSize - 4) {
      /* throw bus timeout exception */
      throwException(EXC_BUS_TIMEOUT);
    }
    data = ((Word) *(ram + (pAddr - RAM_BASE) + 0)) << 24 |
           ((Word) *(ram + (pAddr - RAM_BASE) + 1)) << 16 |
           ((Word) *(ram + (pAddr - RAM_BASE) + 2)) <<  8 |
           ((Word) *(ram + (pAddr - RAM_BASE) + 3)) <<  0;
    *dst++ = data;
    pAddr += 4;
  }
}


void ramWrite(Word pAddr, Word *src, int nWords) {
  Word data;

  while (nWords--) {
    if (pAddr > RAM_BASE + ramSize - 4) {
      /* throw bus timeout exception */
      throwException(EXC_BUS_TIMEOUT);
    }
    data = *src++;
    *(ram + (pAddr - RAM_BASE) + 0) = (Byte) (data >> 24);
    *(ram + (pAddr - RAM_BASE) + 1) = (Byte) (data >> 16);
    *(ram + (pAddr - RAM_BASE) + 2) = (Byte) (data >>  8);
    *(ram + (pAddr - RAM_BASE) + 3) = (Byte) (data >>  0);
    pAddr += 4;
  }
}


void ramReset(void) {
  unsigned int i;

  cPrintf("Resetting RAM...\n");
  for (i = 0; i < ramSize; i++) {
    ram[i] = rand();
  }
  cPrintf("%6d MB RAM installed", ramSize / M);
  if (progImage != NULL) {
    fseek(progImage, 0, SEEK_SET);
    if (fread(ram + progAddr, progSize, 1, progImage) != 1) {
      error("cannot read program image file");
    }
    cPrintf(", %d bytes loaded", progSize);
  }
  cPrintf(".\n");
}


void ramInit(unsigned int mainMemorySize,
             char *progImageName,
             unsigned int progLoadAddr) {
  /* allocate RAM */
  ramSize = mainMemorySize;
  ram = malloc(ramSize);
  if (ram == NULL) {
    error("cannot allocate RAM");
  }
  /* possibly load program image */
  if (progImageName == NULL) {
    /* no program to load */
    progImage = NULL;
  } else {
    /* load program */
    progImage = fopen(progImageName, "rb");
    if (progImage == NULL) {
      error("cannot open program file '%s'", progImageName);
    }
    fseek(progImage, 0, SEEK_END);
    progSize = ftell(progImage);
    progAddr = progLoadAddr;
    if (progAddr + progSize > ramSize) {
      error("program file or load address too big");
    }
    /* do actual loading of image in ramReset() */
  }
  ramReset();
}


void ramExit(void) {
  free(ram);
}
