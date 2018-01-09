/*
 * ram.c -- RAM simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "ram.h"


void ramRead(Word pAddr, Word *data, int nWords) {
}


void ramWrite(Word pAddr, Word *data, int nWords) {
}


void ramReset(void) {
  cPrintf("Resetting RAM...\n");
}


void ramInit(void) {
  ramReset();
}


void ramExit(void) {
}
