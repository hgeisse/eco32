//
// eco32.v -- ECO32 top-level description
//


`timescale 1ns/10ps
`default_nettype none


module eco32(clk_in,
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
             ps2_0_clk,
             ps2_0_data,
             ps2_1_clk,
             ps2_1_data,
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
    inout ps2_0_clk;
    inout ps2_0_data;
    // mouse
    inout ps2_1_clk;
    inout ps2_1_data;
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
  wire clk_ok;				// system clocks stable
  wire clk2;				// system clock, 100 MHz
  wire clk;				// system clock, 50 MHz
  wire rst;				// system reset
  // cpu
  wire bus_stb;				// bus strobe
  wire bus_we;				// bus write enable
  wire [31:2] bus_addr;			// bus address (word address)
  wire [31:0] bus_din;			// bus data input, for reads
  wire [31:0] bus_dout;			// bus data output, for writes
  wire bus_ack;				// bus acknowledge
  wire [15:0] bus_irq;			// bus interrupt requests
  // ram
  wire ram_stb;				// ram strobe
  wire [31:0] ram_dout;			// ram data output
  wire ram_ack;				// ram acknowledge
  // rom
  wire rom_stb;				// rom strobe
  wire [31:0] rom_dout;			// rom data output
  wire rom_ack;				// rom acknowledge
  // i/o
  wire i_o_stb;				// i/o strobe
  // tmr 0
  wire tmr_0_stb;			// tmr 0 strobe
  wire [31:0] tmr_0_dout;		// tmr 0 data output
  wire tmr_0_ack;			// tmr 0 acknowledge
  wire tmr_0_irq;			// tmr 0 interrupt request
  // tmr 1
  wire tmr_1_stb;			// tmr 1 strobe
  wire [31:0] tmr_1_dout;		// tmr 1 data output
  wire tmr_1_ack;			// tmr 1 acknowledge
  wire tmr_1_irq;			// tmr 1 interrupt request
  // dsp
  wire dsp_stb;				// dsp strobe
  wire [15:0] dsp_dout;			// dsp data output
  wire dsp_ack;				// dsp acknowledge
  // kbd
  wire ps2_0_stb;			// kbd strobe
  wire [7:0] ps2_0_dout;		// kbd data output
  wire ps2_0_ack;			// kbd acknowledge
  wire ps2_0_irq;			// kbd interrupt request
  // mouse
  wire ps2_1_stb;			// mouse strobe
  wire [7:0] ps2_1_dout;		// mouse data output
  wire ps2_1_ack;			// mouse acknowledge
  wire ps2_1_irq;			// mouse interrupt request
  // ser 0
  wire ser_0_stb;			// ser 0 strobe
  wire [7:0] ser_0_dout;		// ser 0 data output
  wire ser_0_ack;			// ser 0 acknowledge
  wire ser_0_irq_r;			// ser 0 rcv interrupt request
  wire ser_0_irq_t;			// ser 0 xmt interrupt request
  // ser 1
  wire ser_1_stb;			// ser 1 strobe
  wire [7:0] ser_1_dout;		// ser 1 data output
  wire ser_1_ack;			// ser 1 acknowledge
  wire ser_1_irq_r;			// ser 1 rcv interrupt request
  wire ser_1_irq_t;			// ser 1 xmt interrupt request
  // sdc
  wire sdc_stb;				// sdc strobe
  wire [31:0] sdc_dout;			// sdc data output
  wire sdc_ack;				// sdc acknowledge
  // bio
  wire bio_stb;				// bio strobe
  wire [31:0] bio_dout;			// bio data output
  wire bio_ack;				// bio acknowledge

  //--------------------------------------
  // module instances
  //--------------------------------------

  clk_rst clk_rst_0(
    .clk_in(clk_in),
    .rst_in_n(rst_in_n),
    .clk_ok(clk_ok),
    .clk_100_ps(sdram_clk),
    .clk_100(clk2),
    .clk_50(clk),
    .rst(rst)
  );

  cpu cpu_0(
    .clk(clk),
    .rst(rst),
    .bus_stb(bus_stb),
    .bus_we(bus_we),
    .bus_addr(bus_addr[31:2]),
    .bus_din(bus_din[31:0]),
    .bus_dout(bus_dout[31:0]),
    .bus_ack(bus_ack),
    .bus_irq(bus_irq[15:0])
  );

  ram ram_0(
    .clk_ok(clk_ok),
    .clk2(clk2),
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(bus_we),
    .addr(bus_addr[26:2]),
    .data_in(bus_dout[31:0]),
    .data_out(ram_dout[31:0]),
    .ack(ram_ack),
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

  rom rom_0(
    .clk(clk),
    .rst(rst),
    .stb(rom_stb),
    .we(bus_we),
    .addr(bus_addr[22:2]),
    .data_out(rom_dout[31:0]),
    .ack(rom_ack),
    .ce_n(fl_ce_n),
    .oe_n(fl_oe_n),
    .we_n(fl_we_n),
    .wp_n(fl_wp_n),
    .rst_n(fl_rst_n),
    .a(fl_addr[22:0]),
    .d(fl_dq[7:0])
  );

  tmr tmr_0(
    .clk(clk),
    .rst(rst),
    .stb(tmr_0_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[31:0]),
    .data_out(tmr_0_dout[31:0]),
    .ack(tmr_0_ack),
    .irq(tmr_0_irq)
  );

  tmr tmr_1(
    .clk(clk),
    .rst(rst),
    .stb(tmr_1_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[31:0]),
    .data_out(tmr_1_dout[31:0]),
    .ack(tmr_1_ack),
    .irq(tmr_1_irq)
  );

  dsp dsp_0(
    .clk(clk),
    .rst(rst),
    .stb(dsp_stb),
    .we(bus_we),
    .addr(bus_addr[13:2]),
    .data_in(bus_dout[15:0]),
    .data_out(dsp_dout[15:0]),
    .ack(dsp_ack),
    .hsync(vga_hsync),
    .vsync(vga_vsync),
    .pxclk(vga_clk),
    .sync_n(vga_sync_n),
    .blank_n(vga_blank_n),
    .r(vga_r[7:0]),
    .g(vga_g[7:0]),
    .b(vga_b[7:0])
  );

  ps2 ps2_0(
    .clk(clk),
    .rst(rst),
    .stb(ps2_0_stb),
    .we(bus_we),
    .addr(bus_addr[2]),
    .data_in(bus_dout[7:0]),
    .data_out(ps2_0_dout[7:0]),
    .ack(ps2_0_ack),
    .irq(ps2_0_irq),
    .ps2_clk(ps2_0_clk),
    .ps2_data(ps2_0_data)
  );

  ps2 ps2_1(
    .clk(clk),
    .rst(rst),
    .stb(ps2_1_stb),
    .we(bus_we),
    .addr(bus_addr[2]),
    .data_in(bus_dout[7:0]),
    .data_out(ps2_1_dout[7:0]),
    .ack(ps2_1_ack),
    .irq(ps2_1_irq),
    .ps2_clk(ps2_1_clk),
    .ps2_data(ps2_1_data)
  );

  ser ser_0(
    .clk(clk),
    .rst(rst),
    .stb(ser_0_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[7:0]),
    .data_out(ser_0_dout[7:0]),
    .ack(ser_0_ack),
    .irq_r(ser_0_irq_r),
    .irq_t(ser_0_irq_t),
    .rxd(rs232_0_rxd),
    .txd(rs232_0_txd)
  );

  ser ser_1(
    .clk(clk),
    .rst(rst),
    .stb(ser_1_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[7:0]),
    .data_out(ser_1_dout[7:0]),
    .ack(ser_1_ack),
    .irq_r(ser_1_irq_r),
    .irq_t(ser_1_irq_t),
    .rxd(rs232_1_rxd),
    .txd(rs232_1_txd)
  );

  sdc sdc_0(
    .clk(clk),
    .rst(rst),
    .stb(sdc_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[31:0]),
    .data_out(sdc_dout[31:0]),
    .ack(sdc_ack),
    .ss_n(sdcard_ss_n),
    .sclk(sdcard_sclk),
    .mosi(sdcard_mosi),
    .miso(sdcard_miso),
    .wp(sdcard_wp)
  );

  bio bio_0(
    .clk(clk),
    .rst(rst),
    .stb(bio_stb),
    .we(bus_we),
    .addr(bus_addr[5:2]),
    .data_in(bus_dout[31:0]),
    .data_out(bio_dout[31:0]),
    .ack(bio_ack),
    .led_g(led_g[8:0]),
    .led_r(led_r[17:0]),
    .hex7_n(hex7_n[6:0]),
    .hex6_n(hex6_n[6:0]),
    .hex5_n(hex5_n[6:0]),
    .hex4_n(hex4_n[6:0]),
    .hex3_n(hex3_n[6:0]),
    .hex2_n(hex2_n[6:0]),
    .hex1_n(hex1_n[6:0]),
    .hex0_n(hex0_n[6:0]),
    .key3_n(key3_n),
    .key2_n(key2_n),
    .key1_n(key1_n),
    .sw(sw[17:0])
  );

  //--------------------------------------
  // address decoder
  //--------------------------------------

  // RAM: architectural limit  = 512 MB
  //      implementation limit = 128 MB
  assign ram_stb =
    (bus_stb == 1'b1 && bus_addr[31:29] == 3'b000
                     && bus_addr[28:27] == 2'b00) ? 1'b1 : 1'b0;

  // ROM: architectural limit  = 256 MB
  //      implementation limit =   8 MB
  assign rom_stb =
    (bus_stb == 1'b1 && bus_addr[31:28] == 4'b0010
                     && bus_addr[27:23] == 5'b00000) ? 1'b1 : 1'b0;

  // I/O: architectural limit  = 256 MB
  assign i_o_stb =
    (bus_stb == 1'b1 && bus_addr[31:28] == 4'b0011) ? 1'b1 : 1'b0;
  assign tmr_0_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h00
                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;
  assign tmr_1_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h00
                     && bus_addr[19:12] == 8'h01) ? 1'b1 : 1'b0;
  assign dsp_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h01) ? 1'b1 : 1'b0;
  assign ps2_0_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h02
                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;
  assign ps2_1_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h02
                     && bus_addr[19:12] == 8'h01) ? 1'b1 : 1'b0;
  assign ser_0_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h03
                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;
  assign ser_1_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h03
                     && bus_addr[19:12] == 8'h01) ? 1'b1 : 1'b0;
//  assign dsk_stb =
//    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h04) ? 1'b1 : 1'b0;
//  assign fms_stb =
//    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h05
//                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;
  assign sdc_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h06
                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;
  assign bio_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h10
                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;

  //--------------------------------------
  // data and acknowledge multiplexers
  //--------------------------------------

  assign bus_din[31:0] =
    (ram_stb == 1'b1)   ? ram_dout[31:0] :
    (rom_stb == 1'b1)   ? rom_dout[31:0] :
    (tmr_0_stb == 1'b1) ? tmr_0_dout[31:0] :
    (tmr_1_stb == 1'b1) ? tmr_1_dout[31:0] :
    (dsp_stb == 1'b1)   ? { 16'h0000, dsp_dout[15:0] } :
    (ps2_0_stb == 1'b1) ? { 24'h000000, ps2_0_dout[7:0] } :
    (ps2_1_stb == 1'b1) ? { 24'h000000, ps2_1_dout[7:0] } :
    (ser_0_stb == 1'b1) ? { 24'h000000, ser_0_dout[7:0] } :
    (ser_1_stb == 1'b1) ? { 24'h000000, ser_1_dout[7:0] } :
//    (dsk_stb == 1'b1)   ? dsk_dout[31:0] :
//    (fms_stb == 1'b1)   ? fms_dout[31:0] :
    (sdc_stb == 1'b1)   ? sdc_dout[31:0] :
    (bio_stb == 1'b1)   ? bio_dout[31:0] :
    32'h00000000;

  assign bus_ack =
    (ram_stb == 1'b1)   ? ram_ack :
    (rom_stb == 1'b1)   ? rom_ack :
    (tmr_0_stb == 1'b1) ? tmr_0_ack :
    (tmr_1_stb == 1'b1) ? tmr_1_ack :
    (dsp_stb == 1'b1)   ? dsp_ack :
    (ps2_0_stb == 1'b1) ? ps2_0_ack :
    (ps2_1_stb == 1'b1) ? ps2_1_ack :
    (ser_0_stb == 1'b1) ? ser_0_ack :
    (ser_1_stb == 1'b1) ? ser_1_ack :
//    (dsk_stb == 1'b1)   ? dsk_ack :
//    (fms_stb == 1'b1)   ? fms_ack :
    (sdc_stb == 1'b1)   ? sdc_ack :
    (bio_stb == 1'b1)   ? bio_ack :
    1'b0;

  //--------------------------------------
  // bus interrupt request assignments
  //--------------------------------------

  assign bus_irq[15] = tmr_1_irq;
  assign bus_irq[14] = tmr_0_irq;
  assign bus_irq[13] = 1'b0;
  assign bus_irq[12] = 1'b0;
  assign bus_irq[11] = 1'b0;
  assign bus_irq[10] = 1'b0;
  assign bus_irq[ 9] = 1'b0;
  assign bus_irq[ 8] = 1'b0;
  assign bus_irq[ 7] = 1'b0;
  assign bus_irq[ 6] = 1'b0;
  assign bus_irq[ 5] = ps2_1_irq;
  assign bus_irq[ 4] = ps2_0_irq;
  assign bus_irq[ 3] = ser_1_irq_r;
  assign bus_irq[ 2] = ser_1_irq_t;
  assign bus_irq[ 1] = ser_0_irq_r;
  assign bus_irq[ 0] = ser_0_irq_t;

endmodule
