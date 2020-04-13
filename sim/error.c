/*
 * error.c -- error handler
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "cpu.h"
#include "fpu.h"
#include "trace.h"
#include "mmu.h"
#include "icache.h"
#include "dcache.h"
#include "ram.h"
#include "rom.h"
#include "timer.h"
#include "dsp.h"
#include "kbd.h"
#include "serial.h"
#include "disk.h"
#include "sdcard.h"
#include "output.h"
#include "shutdown.h"
#include "graph1.h"
#include "graph2.h"
#include "mouse.h"


void error(char *fmt, ...) {
  va_list ap;

  cpuExit();
  fpuExit();
  traceExit();
  mmuExit();
  icacheExit();
  dcacheExit();
  ramExit();
  romExit();
  timerExit();
  displayExit();
  graph1Exit();
  graph2Exit();
  keyboardExit();
  mouseExit();
  serialExit();
  diskExit();
  sdcardExit();
  outputExit();
  shutdownExit();
  cExit();
  va_start(ap, fmt);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}
