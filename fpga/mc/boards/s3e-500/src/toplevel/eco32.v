//
// eco32.v -- ECO32 top-level description
//


`timescale 1ns/10ps
`default_nettype none


module eco32(clk_in,
             rst_in,
             sdram_ck_p,
             sdram_ck_n,
             sdram_cke,
             sdram_cs_n,
             sdram_ras_n,
             sdram_cas_n,
             sdram_we_n,
             sdram_ba,
             sdram_a,
             sdram_udm,
             sdram_ldm,
             sdram_udqs,
             sdram_ldqs,
             sdram_dq,
             flash_ce_n,
             flash_oe_n,
             flash_we_n,
             flash_byte_n,
             flash_a,
             flash_d,
             vga_hsync,
             vga_vsync,
             vga_r,
             vga_g,
             vga_b,
             ps2_clk,
             ps2_data,
             rs232_0_rxd,
             rs232_0_txd,
             rs232_1_rxd,
             rs232_1_txd,
             spi_sck,
             spi_mosi,
             dac_cs_n,
             dac_clr_n,
             amp_cs_n,
             amp_shdn,
             ad_conv,
             sw,
             led,
             lcd_e,
             lcd_rw,
             lcd_rs,
             spi_ss_b,
             fpga_init_b);
    // clock and reset
    input clk_in;
    input rst_in;
    // SDRAM
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
    // flash ROM
    output flash_ce_n;
    output flash_oe_n;
    output flash_we_n;
    output flash_byte_n;
    output [23:0] flash_a;
    input [15:0] flash_d;
    // VGA display
    output vga_hsync;
    output vga_vsync;
    output vga_r;
    output vga_g;
    output vga_b;
    // keyboard
    input ps2_clk;
    input ps2_data;
    // serial line 0
    input rs232_0_rxd;
    output rs232_0_txd;
    // serial line 1
    input rs232_1_rxd;
    output rs232_1_txd;
    // SPI bus controller
    output spi_sck;
    output spi_mosi;
    output dac_cs_n;
    output dac_clr_n;
    output amp_cs_n;
    output amp_shdn;
    output ad_conv;
    // board I/O
    input [3:0] sw;
    output [7:0] led;
    output lcd_e;
    output lcd_rw;
    output lcd_rs;
    output spi_ss_b;
    output fpga_init_b;

  // clk_rst
  wire ddr_clk_0;
  wire ddr_clk_90;
  wire ddr_clk_180;
  wire ddr_clk_270;
  wire ddr_clk_ok;
  wire clk;
  wire rst;
  // cpu
  wire bus_stb;
  wire bus_we;
  wire [31:2] bus_addr;
  wire [31:0] bus_din;
  wire [31:0] bus_dout;
  wire bus_ack;
  wire [15:0] bus_irq;
  // ram
  wire ram_stb;
  wire [31:0] ram_dout;
  wire ram_ack;
  // rom
  wire rom_stb;
  wire [31:0] rom_dout;
  wire rom_ack;
  // i/o
  wire i_o_stb;
  // tmr0
  wire tmr0_stb;
  wire [31:0] tmr0_dout;
  wire tmr0_ack;
  wire tmr0_irq;
  // tmr1
  wire tmr1_stb;
  wire [31:0] tmr1_dout;
  wire tmr1_ack;
  wire tmr1_irq;
  // dsp
  wire dsp_stb;
  wire [15:0] dsp_dout;
  wire dsp_ack;
  // kbd
  wire kbd_stb;
  wire [7:0] kbd_dout;
  wire kbd_ack;
  wire kbd_irq;
  // ser0
  wire ser0_stb;
  wire [7:0] ser0_dout;
  wire ser0_ack;
  wire ser0_irq_r;
  wire ser0_irq_t;
  // ser1
  wire ser1_stb;
  wire [7:0] ser1_dout;
  wire ser1_ack;
  wire ser1_irq_r;
  wire ser1_irq_t;
  // fms
  wire fms_stb;
  wire [31:0] fms_dout;
  wire fms_ack;
  // spi
  wire [15:0] dac_sample_l;
  wire [15:0] dac_sample_r;
  wire dac_next;
  // bio
  wire bio_stb;
  wire [31:0] bio_dout;
  wire bio_ack;
  wire spi_en;

  //--------------------------------------
  // module instances
  //--------------------------------------

  clk_rst clk_rst_1(
    .clk_in(clk_in),
    .rst_in(rst_in),
    .ddr_clk_0(ddr_clk_0),
    .ddr_clk_90(ddr_clk_90),
    .ddr_clk_180(ddr_clk_180),
    .ddr_clk_270(ddr_clk_270),
    .ddr_clk_ok(ddr_clk_ok),
    .clk(clk),
    .rst(rst)
  );

  cpu cpu_1(
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

  ram ram_1(
    .ddr_clk_0(ddr_clk_0),
    .ddr_clk_90(ddr_clk_90),
    .ddr_clk_180(ddr_clk_180),
    .ddr_clk_270(ddr_clk_270),
    .ddr_clk_ok(ddr_clk_ok),
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(bus_we),
    .addr(bus_addr[25:2]),
    .data_in(bus_dout[31:0]),
    .data_out(ram_dout[31:0]),
    .ack(ram_ack),
    .sdram_ck_p(sdram_ck_p),
    .sdram_ck_n(sdram_ck_n),
    .sdram_cke(sdram_cke),
    .sdram_cs_n(sdram_cs_n),
    .sdram_ras_n(sdram_ras_n),
    .sdram_cas_n(sdram_cas_n),
    .sdram_we_n(sdram_we_n),
    .sdram_ba(sdram_ba[1:0]),
    .sdram_a(sdram_a[12:0]),
    .sdram_udm(sdram_udm),
    .sdram_ldm(sdram_ldm),
    .sdram_udqs(sdram_udqs),
    .sdram_ldqs(sdram_ldqs),
    .sdram_dq(sdram_dq[15:0])
  );

  rom rom_1(
    .clk(clk),
    .rst(rst),
    .stb(rom_stb),
    .we(bus_we),
    .addr(bus_addr[23:2]),
    .data_out(rom_dout[31:0]),
    .ack(rom_ack),
    .spi_en(spi_en),
    .ce_n(flash_ce_n),
    .oe_n(flash_oe_n),
    .we_n(flash_we_n),
    .byte_n(flash_byte_n),
    .a(flash_a[23:0]),
    .d(flash_d[15:0])
  );

  tmr tmr_1(
    .clk(clk),
    .rst(rst),
    .stb(tmr0_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[31:0]),
    .data_out(tmr0_dout[31:0]),
    .ack(tmr0_ack),
    .irq(tmr0_irq)
  );

  tmr tmr_2(
    .clk(clk),
    .rst(rst),
    .stb(tmr1_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[31:0]),
    .data_out(tmr1_dout[31:0]),
    .ack(tmr1_ack),
    .irq(tmr1_irq)
  );

  dsp dsp_1(
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
    .r(vga_r),
    .g(vga_g),
    .b(vga_b)
  );

  kbd kbd_1(
    .clk(clk),
    .rst(rst),
    .stb(kbd_stb),
    .we(bus_we),
    .addr(bus_addr[2]),
    .data_in(bus_dout[7:0]),
    .data_out(kbd_dout[7:0]),
    .ack(kbd_ack),
    .irq(kbd_irq),
    .ps2_clk(ps2_clk),
    .ps2_data(ps2_data)
  );

  ser ser_1(
    .clk(clk),
    .rst(rst),
    .stb(ser0_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[7:0]),
    .data_out(ser0_dout[7:0]),
    .ack(ser0_ack),
    .irq_r(ser0_irq_r),
    .irq_t(ser0_irq_t),
    .rxd(rs232_0_rxd),
    .txd(rs232_0_txd)
  );

  ser ser_2(
    .clk(clk),
    .rst(rst),
    .stb(ser1_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[7:0]),
    .data_out(ser1_dout[7:0]),
    .ack(ser1_ack),
    .irq_r(ser1_irq_r),
    .irq_t(ser1_irq_t),
    .rxd(rs232_1_rxd),
    .txd(rs232_1_txd)
  );

  fms fms_1(
    .clk(clk),
    .rst(rst),
    .stb(fms_stb),
    .we(bus_we),
    .addr(bus_addr[11:2]),
    .data_in(bus_dout[31:0]),
    .data_out(fms_dout[31:0]),
    .ack(fms_ack),
    .next(dac_next),
    .sample_l(dac_sample_l[15:0]),
    .sample_r(dac_sample_r[15:0])
  );

  spi spi_1(
    .clk(clk),
    .rst(rst),
    .spi_en(spi_en),
    .dac_sample_l(dac_sample_l[15:0]),
    .dac_sample_r(dac_sample_r[15:0]),
    .dac_next(dac_next),
    .spi_sck(spi_sck),
    .spi_mosi(spi_mosi),
    .dac_cs_n(dac_cs_n),
    .dac_clr_n(dac_clr_n),
    .amp_cs_n(amp_cs_n),
    .amp_shdn(amp_shdn),
    .ad_conv(ad_conv)
  );

  bio bio_1(
    .clk(clk),
    .rst(rst),
    .stb(bio_stb),
    .we(bus_we),
    .addr(bus_addr[2]),
    .data_in(bus_dout[31:0]),
    .data_out(bio_dout[31:0]),
    .ack(bio_ack),
    .spi_en(spi_en),
    .sw(sw[3:0]),
    .led(led[7:0]),
    .lcd_e(lcd_e),
    .lcd_rw(lcd_rw),
    .lcd_rs(lcd_rs),
    .spi_ss_b(spi_ss_b),
    .fpga_init_b(fpga_init_b)
  );

  //--------------------------------------
  // address decoder
  //--------------------------------------

  // RAM: architectural limit  = 512 MB
  //      implementation limit =  64 MB
  assign ram_stb =
    (bus_stb == 1 && bus_addr[31:29] == 3'b000
                  && bus_addr[28:26] == 3'b000) ? 1 : 0;

  // ROM: architectural limit  = 256 MB
  //      implementation limit =  16 MB
  assign rom_stb =
    (bus_stb == 1 && bus_addr[31:28] == 4'b0010
                  && bus_addr[27:24] == 4'b0000) ? 1 : 0;

  // I/O: architectural limit  = 256 MB
  assign i_o_stb =
    (bus_stb == 1 && bus_addr[31:28] == 4'b0011) ? 1 : 0;
  assign tmr0_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h00
                  && bus_addr[19:12] == 8'h00) ? 1 : 0;
  assign tmr1_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h00
                  && bus_addr[19:12] == 8'h01) ? 1 : 0;
  assign dsp_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h01) ? 1 : 0;
  assign kbd_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h02) ? 1 : 0;
  assign ser0_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h03
                  && bus_addr[19:12] == 8'h00) ? 1 : 0;
  assign ser1_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h03
                  && bus_addr[19:12] == 8'h01) ? 1 : 0;
  assign fms_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h05
                  && bus_addr[19:12] == 8'h00) ? 1 : 0;
  assign bio_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h10
                  && bus_addr[19:12] == 8'h00) ? 1 : 0;

  //--------------------------------------
  // data and acknowledge multiplexers
  //--------------------------------------

  assign bus_din[31:0] =
    (ram_stb == 1)  ? ram_dout[31:0] :
    (rom_stb == 1)  ? rom_dout[31:0] :
    (tmr0_stb == 1) ? tmr0_dout[31:0] :
    (tmr1_stb == 1) ? tmr1_dout[31:0] :
    (dsp_stb == 1)  ? { 16'h0000, dsp_dout[15:0] } :
    (kbd_stb == 1)  ? { 24'h000000, kbd_dout[7:0] } :
    (ser0_stb == 1) ? { 24'h000000, ser0_dout[7:0] } :
    (ser1_stb == 1) ? { 24'h000000, ser1_dout[7:0] } :
    (fms_stb == 1)  ? fms_dout[31:0] :
    (bio_stb == 1)  ? bio_dout[31:0] :
    32'h00000000;

  assign bus_ack =
    (ram_stb == 1)  ? ram_ack :
    (rom_stb == 1)  ? rom_ack :
    (tmr0_stb == 1) ? tmr0_ack :
    (tmr1_stb == 1) ? tmr1_ack :
    (dsp_stb == 1)  ? dsp_ack :
    (kbd_stb == 1)  ? kbd_ack :
    (ser0_stb == 1) ? ser0_ack :
    (ser1_stb == 1) ? ser1_ack :
    (fms_stb == 1)  ? fms_ack :
    (bio_stb == 1)  ? bio_ack :
    0;

  //--------------------------------------
  // bus interrupt request assignments
  //--------------------------------------

  assign bus_irq[15] = tmr1_irq;
  assign bus_irq[14] = tmr0_irq;
  assign bus_irq[13] = 1'b0;
  assign bus_irq[12] = 1'b0;
  assign bus_irq[11] = 1'b0;
  assign bus_irq[10] = 1'b0;
  assign bus_irq[ 9] = 1'b0;
  assign bus_irq[ 8] = 1'b0;  //dsk_irq;
  assign bus_irq[ 7] = 1'b0;
  assign bus_irq[ 6] = 1'b0;
  assign bus_irq[ 5] = 1'b0;
  assign bus_irq[ 4] = kbd_irq;
  assign bus_irq[ 3] = ser1_irq_r;
  assign bus_irq[ 2] = ser1_irq_t;
  assign bus_irq[ 1] = ser0_irq_r;
  assign bus_irq[ 0] = ser0_irq_t;

endmodule
