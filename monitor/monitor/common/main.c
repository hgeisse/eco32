/*
 * main.c -- the main program
 */


#include "common.h"
#include "stdarg.h"
#include "romlib.h"
#include "command.h"
#include "getline.h"
#include "instr.h"
#include "cpu.h"


int main(void) {
  char *line;

  printf("\n\nECO32 Machine Monitor 1.3\n\n");
  initInstrTable();
  cpuSetPC(0xC0010000);
  cpuSetPSW(0x08000000);
  while (1) {
    line = getLine("ECO32 > ");
    addHist(line);
    execCommand(line);
  }
  return 0;
}
