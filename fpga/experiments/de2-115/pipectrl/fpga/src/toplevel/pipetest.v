//
// pipetest.v -- top-level description
//


`timescale 1ns/10ps
`default_nettype none


module pipetest(
	clk_in,
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
	fl_ce_n,
	fl_oe_n,
	fl_we_n,
	fl_wp_n,
	fl_rst_n,
	fl_addr,
	fl_dq,
	vga_hsync,
	vga_vsync,
	vga_clk,
	vga_sync_n,
	vga_blank_n,
	vga_r,
	vga_g,
	vga_b,
	ps2_clk,
	ps2_data,
	rs232_0_rxd,
	rs232_0_txd,
	rs232_1_rxd,
	rs232_1_txd,
	sdcard_ss_n,
	sdcard_sclk,
	sdcard_mosi,
	sdcard_miso,
	sdcard_wp,
	led_g,
	led_r,
	hex7_n,
	hex6_n,
	hex5_n,
	hex4_n,
	hex3_n,
	hex2_n,
	hex1_n,
	hex0_n,
	key3_n,
	key2_n,
	key1_n,
	sw
    );

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
    // Flash ROM
    output fl_ce_n;
    output fl_oe_n;
    output fl_we_n;
    output fl_wp_n;
    output fl_rst_n;
    output [22:0] fl_addr;
    input [7:0] fl_dq;
    // VGA display
    output vga_hsync;
    output vga_vsync;
    output vga_clk;
    output vga_sync_n;
    output vga_blank_n;
    output [7:0] vga_r;
    output [7:0] vga_g;
    output [7:0] vga_b;
    // keyboard
    input ps2_clk;
    input ps2_data;
    // serial line 0
    input rs232_0_rxd;
    output rs232_0_txd;
    // serial line 1
    input rs232_1_rxd;
    output rs232_1_txd;
    // SD card
    output sdcard_ss_n;
    output sdcard_sclk;
    output sdcard_mosi;
    input sdcard_miso;
    input sdcard_wp;
    // board I/O
    output [8:0] led_g;
    output [17:0] led_r;
    output [6:0] hex7_n;
    output [6:0] hex6_n;
    output [6:0] hex5_n;
    output [6:0] hex4_n;
    output [6:0] hex3_n;
    output [6:0] hex2_n;
    output [6:0] hex1_n;
    output [6:0] hex0_n;
    input key3_n;
    input key2_n;
    input key1_n;
    input [17:0] sw;

  // clk_rst
  wire clk_ok;				// system clock stable
  wire clk;				// system clock, 100 MHz
  wire rst;				// system reset

  //--------------------------------------
  // module instances
  //--------------------------------------

  clk_rst clk_rst_1(
    .clk_in(clk_in),
    .rst_in_n(rst_in_n),
    .clk_ok(clk_ok),
    .clk_100_ps(sdram_clk),
    .clk_100(clk),
    .clk_50(),
    .rst(rst)
  );

  //--------------------------------------
  // temporary external connections
  //--------------------------------------

  // SDRAM
  assign sdram_cke = 1'b0;
  assign sdram_cs_n = 1'b1;
  assign sdram_ras_n = 1'b1;
  assign sdram_cas_n = 1'b1;
  assign sdram_we_n = 1'b1;
  assign sdram_ba[1:0] = 2'b00;
  assign sdram_a[12:0] = 13'h00000;
  assign sdram_dqm[3:0] = 4'h0;
  assign sdram_dq[31:0] = 32'hzzzzzzzz;

  // Flash ROM
  assign fl_ce_n = 1'b1;
  assign fl_oe_n = 1'b1;
  assign fl_we_n = 1'b1;
  assign fl_wp_n = 1'b1;
  assign fl_rst_n = 1'b1;
  assign fl_addr[22:0] = { 15'h0000, fl_dq[7:0]};

  // VGA display
  assign vga_hsync = 1'b0;
  assign vga_vsync = 1'b0;
  assign vga_clk = 1'b0;
  assign vga_sync_n = 1'b1;
  assign vga_blank_n = 1'b1;
  assign vga_r[7:0] = 8'h00;
  assign vga_g[7:0] = 8'h00;
  assign vga_b[7:0] = 8'h00;

  // keyboard
  // see below

  // serial line 0
  assign rs232_0_txd = rs232_0_rxd;

  // serial line 1
  assign rs232_1_txd = rs232_1_rxd;

  // SD card
  assign sdcard_ss_n = 1'b1;
  assign sdcard_sclk = sdcard_wp;
  assign sdcard_mosi = sdcard_miso;

  // board I/O
  assign led_g[8] = ~key3_n & ~key2_n & ~key1_n & ~ps2_clk & ~ps2_data;
  assign led_g[6] = 1'b0;
  assign led_g[5] = 1'b0;
  assign led_g[4] = 1'b0;
  assign led_g[3] = 1'b0;
  assign led_r[17] = | sw[17:0];
  assign led_r[16] = 1'b0;
  assign led_r[15] = 1'b0;
  assign led_r[14] = 1'b0;
  assign led_r[13] = 1'b0;
  assign led_r[12] = 1'b0;
  assign led_r[11] = 1'b0;
  assign led_r[10] = 1'b0;
  assign led_r[9] = 1'b0;
  assign led_r[8] = 1'b0;
  assign led_r[7] = 1'b0;
  assign led_r[6] = 1'b0;
  assign led_r[5] = 1'b0;
  assign led_r[4] = 1'b0;
  assign led_r[3] = 1'b0;
  assign led_r[2] = 1'b0;
  assign led_r[1] = 1'b0;
  assign hex7_n[6:0] = 7'h7F;
  assign hex6_n[6:0] = 7'h7F;
  assign hex5_n[6:0] = 7'h7F;
  assign hex4_n[6:0] = 7'h7F;
  assign hex3_n[6:0] = 7'h7F;
  assign hex2_n[6:0] = 7'h7F;
  assign hex1_n[6:0] = 7'h7F;
  assign hex0_n[6:0] = 7'h7F;

  //--------------------------------------
  // temporary test circuits
  //--------------------------------------

  wire test_ended;
  wire test_error;

  pipe pipe_1(
    .clk(clk),
    .rst(rst),
    .test_ended(test_ended),
    .test_error(test_error)
  );

  //--------------------------------------
  // indicators
  //--------------------------------------

  reg [25:0] heartbeat;

  always @(posedge clk) begin
    heartbeat[25:0] = heartbeat[25:0] + 26'h1;
  end

  assign led_g[0] = clk_ok;
  assign led_g[1] = heartbeat[25];
  assign led_g[2] = rst;
  assign led_g[7] = test_ended;
  assign led_r[0] = test_error;

endmodule
