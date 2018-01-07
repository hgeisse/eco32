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


void icacheReset(void) {
  cPrintf("Resetting Instruction Cache...\n");
}


void icacheInit(void) {
  icacheReset();
}


void icacheExit(void) {
}
