/*
 * fpu.c -- floating-point unit
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common.h"
#include "../console.h"
#include "../error.h"
#include "fpu.h"
#include "fpubits.h"


Word fpc;		/* floating-point control word */


Word fpuGetFPC(void) {
  return fpc;
}


void fpuSetFPC(Word value) {
  fpc = value;
}


void fpuReset(void) {
  cPrintf("Resetting FPU...\n");
  fpc = 0;
}


void fpuInit(void) {
  fpuReset();
}


void fpuExit(void) {
}
