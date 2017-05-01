//
// memtest.v -- top-level for memory test
//


`timescale 1ns/10ps
`default_nettype none


module memtest(clk_in,
               rst_in_n,
               sdram_clk,
               sdram_cke,
               sdram_cs_n,
               sdram_ras_n,
               sdram_cas_n,
               sdram_we_n,
               sdram_ba,
               sdram_a,
               sdram_dqm,
               sdram_dq,
               led_r,
               led_g);

    // clock and reset
    input clk_in;
    input rst_in_n;
    // SDRAM
    output sdram_clk;
    output sdram_cke;
    output sdram_cs_n;
    output sdram_ras_n;
    output sdram_cas_n;
    output sdram_we_n;
    output [1:0] sdram_ba;
    output [12:0] sdram_a;
    output [3:0] sdram_dqm;
    inout [31:0] sdram_dq;
    // LEDs
    output [17:0] led_r;
    output [8:0] led_g;

  // clk_rst
  wire clk_ok;
  wire clk;
  wire rst;
  // ramtest
  wire inst_stb;
  wire [24:0] inst_addr;
  wire [127:0] inst_to_test;
  wire inst_ack;
  wire inst_timeout;
  wire data_stb;
  wire data_we;
  wire [24:0] data_addr;
  wire [127:0] data_to_ram;
  wire [127:0] data_to_test;
  wire data_ack;
  wire data_timeout;
  // indicators
  wire test_ended;
  wire test_error;
  reg [25:0] heartbeat;

  //
  // module instances
  //

  clk_rst clk_rst_1(
    .clk_in(clk_in),
    .rst_in_n(rst_in_n),
    .clk_ok(clk_ok),
    .clk_100_ps(sdram_clk),
    .clk_100(clk),
    .clk_50(),
    .rst(rst)
  );

  ramtest ramtest_1(
    .clk(clk),
    .rst(rst),
    .inst_stb(inst_stb),
    .inst_addr(inst_addr[24:0]),
    .inst_din(inst_to_test[127:0]),
    .inst_ack(inst_ack | inst_timeout),
    .data_stb(data_stb),
    .data_we(data_we),
    .data_addr(data_addr[24:0]),
    .data_dout(data_to_ram[127:0]),
    .data_din(data_to_test[127:0]),
    .data_ack(data_ack | data_timeout),
    .test_ended(test_ended),
    .test_error(test_error)
  );

  ramctrl ramctrl_1(
    .clk_ok(clk_ok),
    .clk(clk),
    .inst_stb(inst_stb),
    .inst_addr(inst_addr[24:0]),
    .inst_dout(inst_to_test[127:0]),
    .inst_ack(inst_ack),
    .inst_timeout(inst_timeout),
    .data_stb(data_stb),
    .data_we(data_we),
    .data_addr(data_addr[24:0]),
    .data_din(data_to_ram[127:0]),
    .data_dout(data_to_test[127:0]),
    .data_ack(data_ack),
    .data_timeout(data_timeout),
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

  //
  // indicators
  //

  always @(posedge clk) begin
    heartbeat[25:0] <= heartbeat[25:0] + 26'd1;
  end

  assign led_r[17:1] = 17'b0;
  assign led_r[0] = test_error;		// test_error

  assign led_g[8] = 1'b0;

  assign led_g[7] = test_ended;		// test_ended
  assign led_g[6] = 1'b0;
  assign led_g[5] = 1'b0;
  assign led_g[4] = 1'b0;
  assign led_g[3] = heartbeat[25];	// heartbeat
  assign led_g[2] = heartbeat[25];	// heartbeat
  assign led_g[1] = clk_ok;		// clk_ok
  assign led_g[0] = rst;		// rst

endmodule
