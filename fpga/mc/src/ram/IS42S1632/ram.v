//
// ram.v -- external RAM interface, using SDRAM
//          32M x 32 bit = 128 MB
//


`timescale 1ns/10ps
`default_nettype none


module ram(clk_ok, clk2, clk, rst,
           stb, we, addr,
           data_in, data_out, ack,
           sdram_cke, sdram_cs_n,
           sdram_ras_n, sdram_cas_n, sdram_we_n,
           sdram_ba, sdram_a, sdram_dqm, sdram_dq);
    // internal interface signals
    input clk_ok;
    input clk2;
    input clk;
    input rst;
    input stb;
    input we;
    input [26:2] addr;
    input [31:0] data_in;
    output [31:0] data_out;
    output ack;
    // external interface signals
    output sdram_cke;
    output sdram_cs_n;
    output sdram_ras_n;
    output sdram_cas_n;
    output sdram_we_n;
    output [1:0] sdram_ba;
    output [12:0] sdram_a;
    output [3:0] sdram_dqm;
    inout [31:0] sdram_dq;

  wire [26:0] ram_addr;

  assign ram_addr[26:0] = { 2'b00, addr[26:2] };

  ramctrl ramctrl_1(
    .clk_ok(clk_ok),
    .clk(clk2),
    .data_stb(stb),
    .data_we(we),
    .data_addr(ram_addr[26:0]),
    .data_din(data_in[31:0]),
    .data_dout(data_out[31:0]),
    .data_ack(ack),
    .data_timeout(),  // ignored, bus timeout is recognized in CPU
    .sdram_cke(sdram_cke),
    .sdram_cs_n(sdram_cs_n),
    .sdram_ras_n(sdram_ras_n),
    .sdram_cas_n(sdram_cas_n),
    .sdram_we_n(sdram_we_n),
    .sdram_ba(sdram_ba[1:0]),
    .sdram_a(sdram_a[12:0]),
    .sdram_dqm(sdram_dqm[3:0]),
    .sdram_dq(sdram_dq[31:0])
  );

endmodule
