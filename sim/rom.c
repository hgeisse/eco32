/*
 * rom.c -- ROM simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "rom.h"


void romRead(Word pAddr, Word *data, int nWords) {
}


void romWrite(Word pAddr, Word *data, int nWords) {
}


void romReset(void) {
  cPrintf("Resetting ROM...\n");
}


void romInit(void) {
  romReset();
}


void romExit(void) {
}
