//
// dac_test.v -- test bench for DAC control circuit
//


`timescale 1ns/10ps
`default_nettype none


module dac_test;

  reg clk;                        // system clock (50 MHz)
  reg rst_in;                     // reset, input
  reg rst_s1;                     // reset, first synchronizer
  reg rst;                        // system reset
  reg [23:0] sample_l;
  reg [23:0] sample_r;
  wire next;
  wire mclk;
  wire bclk;
  wire lrck;
  wire sdti;

  // instantiate the controller
  dac dac_1(clk, rst,
            sample_l, sample_r, next,
            mclk, bclk, lrck, sdti);

  // simulation control
  initial begin
    #0     $dumpfile("dump.vcd");
           $dumpvars(0, dac_test);
           sample_l = 24'h0FF0F6;
           sample_r = 24'hAA55A6;
           clk = 1;
           rst_in = 1;
    #145   rst_in = 0;
    #90000 $finish;
  end

  // clock generator
  always begin
    #10 clk = ~clk;               // 20 nsec cycle time
  end

  // reset synchronizer
  always @(posedge clk) begin
    rst_s1 <= rst_in;
    rst <= rst_s1;
  end

endmodule
