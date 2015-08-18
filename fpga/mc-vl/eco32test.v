//
// eco32test.v -- test bench for ECO32
//


`timescale 1ns/10ps
`default_nettype none


module eco32test(clk_in, rst_in_n);
    input clk_in;		// clock, input, 50 MHz
    input rst_in_n;		// reset, input, active low

  // create an instance of ECO32
  eco32 eco32_1(
    .clk_in(clk_in),
    .rst_in_n(rst_in_n)
  );

endmodule
