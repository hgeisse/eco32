//
// dsp.v -- character display interface
//


`timescale 1ns/10ps
`default_nettype none


module dsp(clk, rst,
           stb, we, addr,
           data_in, data_out, ack);
    input clk;
    input rst;
    input stb;
    input we;
    input [13:2] addr;
    input [15:0] data_in;
    output [15:0] data_out;
    output ack;

  integer dsp_out;		// file handle for display output

  reg [15:0] mem[0:4095];	// 32 x 128 attr/char display memory

  initial begin
    dsp_out = $fopen("dsp.out", "w");
  end

  always @(posedge clk) begin
    if (stb & we) begin
      mem[addr[13:2]] <= data_in[15:0];
      $fdisplay(dsp_out,
                "row = %d, col = %d, attr = 0x%h, char = 0x%h",
                addr[13:9], addr[8:2], data_in[15:8], data_in[7:0]);
    end
  end

  assign data_out[15:0] = mem[addr[13:2]];
  assign ack = stb;

endmodule
