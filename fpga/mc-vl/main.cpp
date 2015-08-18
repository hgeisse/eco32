/*
 * main.cpp -- simulation control
 */


#include "Veco32test.h"
#include "verilated.h"


int main(int argc, char *argv[]) {
  Verilated::commandArgs(argc, argv);
  Veco32test *top = new Veco32test;
  vluint64_t numHalfCycles = 0;
  top->clk_in = 0;
  top->rst_in_n = 0;
  while (numHalfCycles < 2 * 30000000) {
    if (numHalfCycles == 2 * 145) {
      top->rst_in_n = 1;
    }
    top->eval();
    top->clk_in = !top->clk_in;
    numHalfCycles++;
  }
  top->final();
  delete top;
  return 0;
}
