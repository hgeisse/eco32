/*
 * main.cpp -- main program for floating-point test
 */


#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Vfpcmp.h"
#include "verilated.h"


#define LINE_SIZE	200


int main(int argc, char *argv[]) {
  VerilatedContext *contextp;
  Vfpcmp *top;
  char line[LINE_SIZE];
  char *endptr;
  int pred;
  unsigned int x, y;
  bool z;
  unsigned int flags;

  contextp = new VerilatedContext;
  contextp->commandArgs(argc, argv);
  top = new Vfpcmp{contextp};
  top->clk = 0;
  top->run = 0;
  top->eval();
  top->clk = 1;
  top->eval();
  top->clk = 0;
  while (fgets(line, LINE_SIZE, stdin) != NULL) {
    endptr = line;
    pred = strtoul(endptr, &endptr, 10);
    x = strtoul(endptr, &endptr, 16);
    y = strtoul(endptr, &endptr, 16);
    top->pred = pred;
    top->x = x;
    top->y = y;
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
    fprintf(stdout, "%d %02X\n", z, flags);
    fflush(stdout);
  }
  delete top;
  delete contextp;
  return 0;
}
