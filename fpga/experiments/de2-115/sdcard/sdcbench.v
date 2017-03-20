//
// sdcbench.v -- SD card test bench
//


`include "sdctest.v"
`include "sdcctrl.v"


`timescale 1ns/10ps
`default_nettype none


module sdcbench;

  reg clk;
  reg rst_in;
  reg rst;

  wire stb;
  wire we;
  wire addr;
  wire [7:0] test_out;
  wire [7:0] test_in;

  initial begin
    #0       $dumpfile("dump.vcd");
             $dumpvars(0, sdcbench);
             clk = 1;
             rst_in = 1;
    #25      rst_in = 0;
    #100000  $finish;
  end

  always begin
    #10 clk = ~ clk;
  end

  always @(posedge clk) begin
    rst <= rst_in;
  end

  sdctest sdctest_1(
    .clk(clk),
    .rst(rst),
    .stb(stb),
    .we(we),
    .addr(addr),
    .dout(test_out[7:0]),
    .din(test_in[7:0])
  );

  sdcctrl sdcctrl_1(
    .clk(clk),
    .rst(rst),
    .stb(stb),
    .we(we),
    .addr(addr),
    .din(test_out[7:0]),
    .dout(test_in[7:0])
  );

endmodule
