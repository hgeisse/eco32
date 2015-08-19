/*
 * main.cpp -- simulation control
 */


#if VM_TRACE
#define VCD_NAME	"dump.vcd"
#endif

#define DURATION_NAME	"duration.dat"	/* simulation duration file name */
#define RST_IN_OFF	145		/* trailing edge of rst_in, in ns */
#define HALF_PERIOD	10		/* half of clock period, in ns */


#include <iostream>
#include <fstream>
using namespace std;

#include "Veco32test.h"
#include "verilated.h"

#if VM_TRACE
#include "verilated_vcd_c.h"
#endif


int main(int argc, char *argv[]) {
  Verilated::commandArgs(argc, argv);
  Veco32test *top = new Veco32test;
#if VM_TRACE
  Verilated::traceEverOn(true);
  VerilatedVcdC *tfp = new VerilatedVcdC;
  top->trace(tfp, 99);
  tfp->open(VCD_NAME);
#endif
  ifstream durationFile(DURATION_NAME);
  if (!durationFile.is_open()) {
    cout << "Error: cannot open input file '"
         << DURATION_NAME
         << "'\n";
    abort();
  }
  vluint64_t maxTime;
  durationFile >> maxTime;
  maxTime += RST_IN_OFF;
  durationFile.close();
  vluint64_t curTime = 0;
  top->clk_in = 1;
  top->rst_in_n = 0;
  while (curTime <= maxTime) {
    if (top->rst_in_n == 0 && curTime >= RST_IN_OFF) {
      top->rst_in_n = 1;
    }
    top->eval();
#if VM_TRACE
    tfp->dump(curTime);
#endif
    top->clk_in = !top->clk_in;
    curTime += HALF_PERIOD;
  }
  top->final();
#if VM_TRACE
  tfp->close();
  delete tfp;
#endif
  delete top;
  return 0;
}
