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


/**************************************************************/


static Word fpc;		/* floating-point control */


/**************************************************************/


Word fpuAdd(Word x, Word y) {
  error("fpuAdd not yet");
  return 0;
}


Word fpuSub(Word x, Word y) {
  error("fpuSub not yet");
  return 0;
}


Word fpuMul(Word x, Word y) {
  error("fpuMul not yet");
  return 0;
}


Word fpuDiv(Word x, Word y) {
  error("fpuDiv not yet");
  return 0;
}


Word fpuSqrt(Word x) {
  error("fpuSqrt not yet");
  return 0;
}


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
