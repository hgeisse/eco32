//
// sdc.v -- SD card interface
//


`timescale 1ns/10ps
`default_nettype none


module sdc(clk, rst,
           stb, we,
           data_in, data_out,
           ack,
           sdcard_clk, sdcard_cmd,
           sdcard_dat, sdcard_wp);
    // internal interface
    input clk;
    input rst;
    input stb;
    input we;
    input [31:0] data_in;
    output [31:0] data_out;
    output ack;
    // external interface
    output sdcard_clk;
    inout sdcard_cmd;
    inout [3:0] sdcard_dat;
    input sdcard_wp;

  reg [31:0] ctrl;

  always @(posedge clk) begin
    if (rst) begin
      ctrl[31:0] <= 32'h0000FFFF;
    end else begin
      if (stb & we) begin
        ctrl[31:0] <= data_in[31:0];
      end
    end
  end

  assign data_out[31:0] =
    { 25'h0, sdcard_wp, sdcard_clk, sdcard_cmd, sdcard_dat[3:0] };
  assign ack = stb;

  assign sdcard_clk    = ctrl[5];
  assign sdcard_cmd    = ctrl[12] ? 1'bz : ctrl[4];
  assign sdcard_dat[3] = ctrl[11] ? 1'bz : ctrl[3];
  assign sdcard_dat[2] = ctrl[10] ? 1'bz : ctrl[2];
  assign sdcard_dat[1] = ctrl[ 9] ? 1'bz : ctrl[1];
  assign sdcard_dat[0] = ctrl[ 8] ? 1'bz : ctrl[0];

endmodule
