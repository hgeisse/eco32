/*
 * bio.c -- board I/O
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "bio.h"


/*
 * bit vector: { 20'bx, button[3:0], switch[7:0] }
 * button: press = 1
 * switch: on = 1
 */
static Word initSwitches;
static Word currSwitches;


static void showSwitches(void) {
  static char *st[2] = { "off", "on " };
  int i;

  cPrintf("Switches :");
  for (i = 7; i >= 0; i--) {
    cPrintf("  %s", st[(currSwitches >> i) & 1]);
  }
  cPrintf("\n");
  cPrintf("Buttons  :");
  for (i = 11; i >= 8; i--) {
    cPrintf("  %s", st[(currSwitches >> i) & 1]);
  }
  cPrintf("\n");
}


/*
 * bit vector: { 24'bx, led[7:0] }
 * led: 1 = on
 */
static Word currLEDs;


static void showLEDs(void) {
  static char *st[2] = { "OFF", "ON " };
  int i;

  cPrintf("LEDs     :");
  for (i = 7; i >= 0; i--) {
    cPrintf("  %s", st[(currLEDs >> i) & 1]);
  }
  cPrintf("\n");
}


/**************************************************************/


Word bioRead(Word addr) {
  return currSwitches;
}


void bioWrite(Word addr, Word data) {
  data &= 0x000000FF;
  if (currLEDs != data) {
    currLEDs = data;
    showLEDs();
  }
}


/**************************************************************/


void bioShowBoard(void) {
  showLEDs();
  showSwitches();
}


void bioSetSwitches(Word data) {
  currSwitches = data & 0xFFF;
}


void bioReset(void) {
  cPrintf("Resetting Board I/O...\n");
  currLEDs = 0;
  currSwitches = initSwitches;
  showLEDs();
  showSwitches();
}


void bioInit(Word initialSwitches) {
  initSwitches = initialSwitches & 0xFFF;
  bioReset();
}


void bioExit(void) {
}
