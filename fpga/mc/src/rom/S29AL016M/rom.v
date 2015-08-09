//
// rom.v -- parallel flash ROM interface
//          512K x 32 bit = 2 MB
//


`timescale 1ns/10ps
`default_nettype none


module rom(clk, rst,
           stb, we, addr,
           data_out, ack,
           ce_n, oe_n, we_n, rst_n, byte_n, a, d);
    // internal interface signals
    input clk;
    input rst;
    input stb;
    input we;
    input [20:2] addr;
    output reg [31:0] data_out;
    output reg ack;
    // flash ROM interface signals
    output ce_n;
    output oe_n;
    output we_n;
    output rst_n;
    output byte_n;
    output [19:0] a;
    input [15:0] d;

  reg [3:0] state;
  reg upper_half;

  // the following control signals are all
  // either constantly asserted or deasserted
  assign ce_n = 0;
  assign oe_n = 0;
  assign we_n = 1;
  assign rst_n = 1;
  assign byte_n = 1;

  // the flash ROM is organized in 16-bit halfwords
  // address line a[0] is controlled by the state machine
  // ("upper half" means "at higher address in ROM")
  assign a[19:1] = addr[20:2];
  assign a[0] = upper_half;

  // the state machine
  always @(posedge clk) begin
    if (rst) begin
      state <= 0;
      ack <= 0;
    end else begin
      if (state == 0) begin
        // wait for start of access
        if (stb & ~we) begin
          state <= 1;
          upper_half <= 0;
        end
      end else
      if (state == 6) begin
        // latch upper halfword
        data_out[31:24] <= d[7:0];
        data_out[23:16] <= d[15:8];
        state <= 7;
        upper_half <= 1;
      end else
      if (state == 12) begin
        // latch lower halfword
        data_out[15:8] <= d[7:0];
        data_out[7:0] <= d[15:8];
        state <= 13;
        ack <= 1;
      end else
      if (state == 13) begin
        // end of access
        ack <= 0;
        state <= 0;
      end else begin
        // wait for flash ROM access time to pass
        state <= state + 1;
      end
    end
  end

endmodule
