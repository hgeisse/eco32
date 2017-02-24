//
// rom.v -- Flash ROM interface
//          2M x 32 bit = 8 MB
//


`timescale 1ns/10ps
`default_nettype none


module rom(clk, rst,
           stb, we, addr,
           data_out, ack,
           ce_n, oe_n, we_n,
           wp_n, rst_n, a, d);
    // internal interface signals
    input clk;
    input rst;
    input stb;
    input we;
    input [22:2] addr;
    output reg [31:0] data_out;
    output reg ack;
    // external interface signals
    output ce_n;
    output oe_n;
    output we_n;
    output wp_n;
    output rst_n;
    output [22:0] a;
    input [7:0] d;

  reg [1:0] ba;
  reg [4:0] state;

  // the following control signals are all
  // either constantly asserted or deasserted
  assign ce_n = 1'b0;
  assign oe_n = 1'b0;
  assign we_n = 1'b1;
  assign wp_n = 1'b1;
  assign rst_n = 1'b1;

  // the flash ROM is organized in 8-bit bytes, and thus
  // address lines a[1:0] are controlled by the state machine
  assign a[22:2] = addr[22:2];
  assign a[1:0] = ba[1:0];

  // the state machine
  // execute 4 Flash cycles to get at a full word
  always @(posedge clk) begin
    if (rst) begin
      ack <= 1'b0;
      state[4:0] <= 5'd0;
    end else begin
      if (state[4:0] == 5'd0) begin
        // wait for start of access
        if (stb & ~we) begin
          ba[1:0] <= 2'b00;
          state[4:0] <= 5'd1;
        end
      end else
      if (state[4:0] == 5'd6) begin
        data_out[31:24] <= d[7:0];
        ba[1:0] <= 2'b01;
        state[4:0] <= 5'd7;
      end else
      if (state[4:0] == 5'd12) begin
        data_out[23:16] <= d[7:0];
        ba[1:0] <= 2'b10;
        state[4:0] <= 5'd13;
      end else
      if (state[4:0] == 5'd18) begin
        data_out[15:8] <= d[7:0];
        ba[1:0] <= 2'b11;
        state[4:0] <= 5'd19;
      end else
      if (state[4:0] == 5'd24) begin
        data_out[7:0] <= d[7:0];
        ack <= 1'b1;
        state[4:0] <= 5'd25;
      end else
      if (state[4:0] == 5'd25) begin
        // end of access
        ack <= 1'b0;
        state[4:0] <= 5'd0;
      end else begin
        // wait for Flash ROM access time to pass
        state[4:0] <= state[4:0] + 5'd1;
      end
    end
  end

endmodule
