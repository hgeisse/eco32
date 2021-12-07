/*
 * fpu.c -- floating-point unit
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common.h"
#include "../console.h"
#include "../error.h"
#include "../fpu.h"
#include "fpubits.h"


/**************************************************************/


Word fpc;		/* floating-point control word */


/**************************************************************/


Word fpuIntToFloat(Word x) {
  error("fpuIntToFloat not yet");
  return 0;
}


Word fpuFloatToInt(Word x, int round) {
  error("fpuFloatToInt not yet");
  return 0;
}


/**************************************************************/


void fpuCmp(Word x, Word y, int pred) {
  error("fpuCmp not yet");
}


/**************************************************************/


Word fpuGetFPC(void) {
  return fpc;
}


void fpuSetFPC(Word value) {
  fpc = value;
}


/**************************************************************/


void fpuReset(void) {
  cPrintf("Resetting FPU...\n");
  fpc = 0;
}


void fpuInit(void) {
  fpuReset();
}


void fpuExit(void) {
}
