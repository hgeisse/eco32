/*
 * dcache.c -- data cache simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "dcache.h"


void dcacheReset(void) {
  cPrintf("Resetting Data Cache...\n");
}


void dcacheInit(void) {
  dcacheReset();
}


void dcacheExit(void) {
}
