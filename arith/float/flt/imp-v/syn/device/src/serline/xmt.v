//
// xmt.v -- serial line transmitter
//


`timescale 1ns / 1ps
`default_nettype none


module xmt(clk, rst, bit_len, load, empty, parallel_in, serial_out);
    input clk;
    input rst;
    input [15:0] bit_len;
    input load;
    output reg empty;
    input [7:0] parallel_in;
    output serial_out;

  reg [3:0] state;
  reg [8:0] shift;
  reg [15:0] count;

  assign serial_out = shift[0];

  always @(posedge clk) begin
    if (rst) begin
      state <= 4'h0;
      shift <= 9'b111111111;
      empty <= 1'b1;
    end else begin
      if (state == 4'h0) begin
        if (load) begin
          state <= 4'h1;
          shift <= { parallel_in, 1'b0 };
          count <= bit_len;
          empty <= 1'b0;
        end
      end else
      if (state == 4'hb) begin
        state <= 4'h0;
        empty <= 1'b1;
      end else begin
        if (count == 16'd0) begin
          state <= state + 4'h1;
          shift[8:0] <= { 1'b1, shift[8:1] };
          count <= bit_len;
        end else begin
          count <= count - 16'd1;
        end
      end
    end
  end

endmodule
