//
// memtest.v -- top-level for memory test
//


`timescale 1ns/10ps
`default_nettype none


module memtest(clk_in,
               rst_inout_n,
               sdram_clk,
               sdram_fb,
               sdram_cke,
               sdram_cs_n,
               sdram_ras_n,
               sdram_cas_n,
               sdram_we_n,
               sdram_ba,
               sdram_a,
               sdram_udqm,
               sdram_ldqm,
               sdram_dq,
               ssl);

    // clock and reset
    input clk_in;
    inout rst_inout_n;
    // SDRAM
    output sdram_clk;
    input sdram_fb;
    output sdram_cke;
    output sdram_cs_n;
    output sdram_ras_n;
    output sdram_cas_n;
    output sdram_we_n;
    output [1:0] sdram_ba;
    output [12:0] sdram_a;
    output sdram_udqm;
    output sdram_ldqm;
    inout [15:0] sdram_dq;
    // 7 segment LED output
    output [6:0] ssl;

  // clk_rst
  wire clk_ok;
  wire clk2;
  wire clk;
  wire rst;
  // ramctrl
  wire inst_stb;
  wire [25:0] inst_addr;
  wire [63:0] inst_to_test;
  wire inst_ack;
  wire inst_timeout;
  wire data_stb;
  wire data_we;
  wire [25:0] data_addr;
  wire [63:0] data_to_ram;
  wire [63:0] data_to_test;
  wire data_ack;
  wire data_timeout;
  // ramtest
  wire test_ended;
  wire test_error;
  reg [25:0] heartbeat2;
  reg [25:0] heartbeat;

  //
  // module instances
  //

  clk_rst clk_rst_1(
    .clk_in(clk_in),
    .rst_inout_n(rst_inout_n),
    .sdram_clk(sdram_clk),
    .sdram_fb(sdram_fb),
    .clk_ok(clk_ok),
    .clk2(clk2),
    .clk(clk),
    .rst(rst)
  );

  ramctrl ramctrl_1(
    .clk_ok(clk_ok),
    .clk(clk2),
    .inst_stb(inst_stb),
    .inst_addr({ test_ended ^ inst_addr[25], inst_addr[24:0] }),
    .inst_dout(inst_to_test[63:0]),
    .inst_ack(inst_ack),
    .inst_timeout(inst_timeout),
    .data_stb(data_stb),
    .data_we(data_we),
    .data_addr({ test_ended ^ data_addr[25], data_addr[24:0] }),
    .data_din(data_to_ram[63:0]),
    .data_dout(data_to_test[63:0]),
    .data_ack(data_ack),
    .data_timeout(data_timeout),
    .sdram_cke(sdram_cke),
    .sdram_cs_n(sdram_cs_n),
    .sdram_ras_n(sdram_ras_n),
    .sdram_cas_n(sdram_cas_n),
    .sdram_we_n(sdram_we_n),
    .sdram_ba(sdram_ba[1:0]),
    .sdram_a(sdram_a[12:0]),
    .sdram_udqm(sdram_udqm),
    .sdram_ldqm(sdram_ldqm),
    .sdram_dq(sdram_dq[15:0])
  );

  ramtest ramtest_1(
    .clk(clk),
    .rst(rst),
    .inst_stb(inst_stb),
    .inst_addr(inst_addr[25:0]),
    .inst_din(inst_to_test[63:0]),
    .inst_ack(inst_ack | inst_timeout),
    .data_stb(data_stb),
    .data_we(data_we),
    .data_addr(data_addr[25:0]),
    .data_dout(data_to_ram[63:0]),
    .data_din(data_to_test[63:0]),
    .data_ack(data_ack | data_timeout),
    .test_ended(test_ended),
    .test_error(test_error)
  );

  always @(posedge clk2) begin
    heartbeat2 <= heartbeat2 + 1;
  end

  always @(posedge clk) begin
    heartbeat <= heartbeat + 1;
  end

  assign ssl[0] = heartbeat2[25];
  assign ssl[1] = clk_ok;
  assign ssl[2] = rst;
  assign ssl[3] = heartbeat[25];
  assign ssl[4] = test_ended;
  assign ssl[5] = test_error;
  assign ssl[6] = 0;

endmodule
