/*
 * console.c -- the simulator's operator console
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "cpu.h"

#include "getline.h"


static Bool controlledByExpect;
static void (*oldSigIntHandler)(int signum);


static void newSigIntHandler(int signum) {
  signal(SIGINT, newSigIntHandler);
  cpuHalt();
}


char *cGetLine(char *prompt) {
  return gl_getline(prompt);
}


void cAddHist(char *line) {
  gl_histadd(line);
}


void cPrintf(char *format, ...) {
  va_list ap;

  if (controlledByExpect) {
    /* don't output any simulator messages */
    return;
  }
  va_start(ap, format);
  vprintf(format, ap);
  va_end(ap);
}


void cInit(Bool expect) {
  controlledByExpect = expect;
  oldSigIntHandler = signal(SIGINT, newSigIntHandler);
}


void cExit(void) {
  signal(SIGINT, oldSigIntHandler);
}
