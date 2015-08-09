//
// fms.v -- FM synthesizer
//
// NOTE: This is a fake module for now.
//       It allows writing directly to the DAC.
//


`timescale 1ns/10ps
`default_nettype none


module fms(clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack,
           next, sample_l, sample_r);
    // internal interface
    input clk;
    input rst;
    input stb;
    input we;
    input [11:2] addr;
    input [31:0] data_in;
    output [31:0] data_out;
    output ack;
    // DAC controller interface
    input next;
    output [15:0] sample_l;
    output [15:0] sample_r;

  reg [31:0] value;
  reg value_needed;

  always @(posedge clk) begin
    if (rst) begin
      value[31:0] <= 32'h0;
      value_needed <= 0;
    end else begin
      if (stb & we & ~|addr[11:2]) begin
        value[31:0] <= data_in[31:0];
        value_needed <= 0;
      end else begin
        if (next) begin
          value_needed <= 1;
        end
      end
    end
  end

  assign data_out[31:0] = { 31'h0, value_needed };
  assign ack = stb;

  assign sample_l[15:0] = value[31:16];
  assign sample_r[15:0] = value[15:0];

endmodule
