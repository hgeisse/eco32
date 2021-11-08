/*
 * main.cpp -- main program for floating-point test
 */


#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Vfpcif.h"
#include "verilated.h"


#define LINE_SIZE	200


int main(int argc, char *argv[]) {
  VerilatedContext *contextp;
  Vfpcif *top;
  char line[LINE_SIZE];
  char *endptr;
  unsigned int x;
  unsigned int z;
  unsigned int flags;

  contextp = new VerilatedContext;
  contextp->commandArgs(argc, argv);
  top = new Vfpcif{contextp};
  top->clk = 0;
  top->run = 0;
  top->eval();
  top->clk = 1;
  top->eval();
  top->clk = 0;
  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    endptr = line;
    x = strtoul(endptr, &endptr, 16);
    top->x = x;
    top->run = 1;
    top->eval();
    top->clk = 1;
    top->eval();
    top->clk = 0;
    while (top->stall == 1) {
      top->eval();
      top->clk = 1;
      top->eval();
      top->clk = 0;
    }
    z = top->z;
    flags = top->flags;
    fprintf(stdout, "%08X %02X\n", z, flags);
    fflush(stdout);
  }
  delete top;
  delete contextp;
  return 0;
}
