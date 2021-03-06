//
// memtest.v -- test bench for memory controller
//


`include "ramtest/ramtest.v"
`include "ramctrl/ramctrl.v"


`timescale 1ns/10ps
`default_nettype none


module memtest;

  reg clk2;			// system clock, 100 MHz
  reg clk;			// system clock, 50 MHz
  reg clk_ok_in;		// clocks are stable, input
  reg clk_ok;			// system clocks are stable
  reg rst_in;			// reset, input
  reg rst;			// system reset

  wire data_stb;
  wire data_we;
  wire [26:0] data_addr;
  wire [31:0] data_to_mctrl;
  wire [31:0] data_to_cache;
  wire data_ack;
  wire data_timeout;

  wire test_ended;
  wire test_error;

  // simulation control
  initial begin
    #0          $timeformat(-9, 1, " ns", 12);
                $dumpfile("dump.vcd");
                $dumpvars(0, memtest);
                clk2 = 1;
                clk = 0;
                clk_ok_in = 0;
                rst_in = 1;
    #43         clk_ok_in = 1;
    #200400     rst_in = 0;
    #415400     $finish;
  end

  // clock generator, 100 MHz
  always begin
    #5 clk2 = ~clk2;		// 10 nsec cycle time
  end

  // clock generator, 50 MHz
  always begin
    #10 clk = ~clk;		// 20 nsec cycle time
  end

  // clk_ok synchronizer
  always @(posedge clk) begin
    clk_ok <= clk_ok_in;
  end

  // reset synchronizer
  always @(posedge clk) begin
    rst <= rst_in;
  end

  ramtest ramtest_1(
    .clk(clk),
    .rst(rst),
    .data_stb(data_stb),
    .data_we(data_we),
    .data_addr(data_addr[26:0]),
    .data_dout(data_to_mctrl[31:0]),
    .data_din(data_to_cache[31:0]),
    .data_ack(data_ack | data_timeout),
    .test_ended(test_ended),
    .test_error(test_error)
  );

  ramctrl ramctrl_1(
    .clk_ok(clk_ok),
    .clk(clk2),
    .data_stb(data_stb),
    .data_we(data_we),
    .data_addr(data_addr[26:0]),
    .data_din(data_to_mctrl[31:0]),
    .data_dout(data_to_cache[31:0]),
    .data_ack(data_ack),
    .data_timeout(data_timeout)
  );

endmodule
