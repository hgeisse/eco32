//
// ram.v -- external RAM interface, using DDR SDRAM
//          16M x 32 bit = 64 MB
//


`timescale 1ns/10ps
`default_nettype none


module ram(ddr_clk_0, ddr_clk_90, ddr_clk_180,
           ddr_clk_270, ddr_clk_ok, clk, rst,
           stb, we, addr, data_in, data_out, ack,
           sdram_ck_p, sdram_ck_n, sdram_cke,
           sdram_cs_n, sdram_ras_n, sdram_cas_n,
           sdram_we_n, sdram_ba, sdram_a,
           sdram_udm, sdram_ldm,
           sdram_udqs, sdram_ldqs, sdram_dq);
    // internal interface signals
    input ddr_clk_0;
    input ddr_clk_90;
    input ddr_clk_180;
    input ddr_clk_270;
    input ddr_clk_ok;
    input clk;
    input rst;
    input stb;
    input we;
    input [25:2] addr;
    input [31:0] data_in;
    output [31:0] data_out;
    output ack;
    // DDR SDRAM interface signals
    output sdram_ck_p;
    output sdram_ck_n;
    output sdram_cke;
    output sdram_cs_n;
    output sdram_ras_n;
    output sdram_cas_n;
    output sdram_we_n;
    output [1:0] sdram_ba;
    output [12:0] sdram_a;
    output sdram_udm;
    output sdram_ldm;
    inout sdram_udqs;
    inout sdram_ldqs;
    inout [15:0] sdram_dq;

  ddr_sdram ddr_sdram_1(
    .sd_CK_P(sdram_ck_p),
    .sd_CK_N(sdram_ck_n),
    .sd_A_O(sdram_a[12:0]),
    .sd_BA_O(sdram_ba[1:0]),
    .sd_D_IO(sdram_dq[15:0]),
    .sd_RAS_O(sdram_ras_n),
    .sd_CAS_O(sdram_cas_n),
    .sd_WE_O(sdram_we_n),
    .sd_UDM_O(sdram_udm),
    .sd_LDM_O(sdram_ldm),
    .sd_UDQS_IO(sdram_udqs),
    .sd_LDQS_IO(sdram_ldqs),
    .sd_CS_O(sdram_cs_n),
    .sd_CKE_O(sdram_cke),
    .clk0(ddr_clk_0),
    .clk90(ddr_clk_90),
    .clk180(ddr_clk_180),
    .clk270(ddr_clk_270),
    .reset(~ddr_clk_ok),
    .wADR_I(addr[25:2]),
    .wSTB_I(stb),
    .wWE_I(we),
    .wWRB_I(4'b1111),
    .wDAT_I(data_in[31:0]),
    .wDAT_O(data_out[31:0]),
    .wACK_O(ack)
  );

endmodule
