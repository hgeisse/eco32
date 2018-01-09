/*
 * icache.c -- instruction cache simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "icache.h"
#include "ram.h"
#include "rom.h"


Word icacheReadWord(Word pAddr) {
  Word data;

  if ((pAddr & 0x30000000) == ROM_BASE) {
    romRead(pAddr, &data, 1);
  } else {
    ramRead(pAddr, &data, 1);
  }
  return data;
}


void icacheReset(void) {
  cPrintf("Resetting Instruction Cache...\n");
}


void icacheInit(void) {
  icacheReset();
}


void icacheExit(void) {
}
