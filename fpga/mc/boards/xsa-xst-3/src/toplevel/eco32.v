//
// eco32.v -- ECO32 top-level description
//


`timescale 1ns/10ps
`default_nettype none


module eco32(clk_in,
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
             flash_ce_n,
             flash_oe_n,
             flash_we_n,
             flash_rst_n,
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
             pbus_d,
             pbus_a,
             pbus_read_n,
             pbus_write_n,
             ata_cs0_n,
             ata_cs1_n,
             ata_intrq,
             ata_dmarq,
             ata_dmack_n,
             ata_iordy,
             dac_mclk,
             dac_sclk,
             dac_lrck,
             dac_sdti,
             slot1_cs_n,
             slot2_cs_n,
             ether_cs_n,
             sw1_3,
             sw1_4,
             sw2_n,
             sw3_n);

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
    // flash ROM
    output flash_ce_n;
    output flash_oe_n;
    output flash_we_n;
    output flash_rst_n;
    output flash_byte_n;
    output [19:0] flash_a;
    input [15:0] flash_d;
    // VGA display
    output vga_hsync;
    output vga_vsync;
    output [2:0] vga_r;
    output [2:0] vga_g;
    output [2:0] vga_b;
    // keyboard
    input ps2_clk;
    input ps2_data;
    // serial line 0
    input rs232_0_rxd;
    output rs232_0_txd;
    // serial line 1
    input rs232_1_rxd;
    output rs232_1_txd;
    // peripheral bus
    inout [15:0] pbus_d;
    output [4:0] pbus_a;
    output pbus_read_n;
    output pbus_write_n;
    // ATA adapter
    output ata_cs0_n;
    output ata_cs1_n;
    input ata_intrq;
    input ata_dmarq;
    output ata_dmack_n;
    input ata_iordy;
    // audio DAC
    output dac_mclk;
    output dac_sclk;
    output dac_lrck;
    output dac_sdti;
    // expansion slot 1
    output slot1_cs_n;
    // expansion slot 2
    output slot2_cs_n;
    // ethernet
    output ether_cs_n;
    // board I/O
    input sw1_3;
    input sw1_4;
    input sw2_n;
    input sw3_n;

  // clk_rst
  wire clk;				// system clock
  wire clk_ok;				// clock is stable
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
  // tmr0
  wire tmr0_stb;			// tmr 0 strobe
  wire [31:0] tmr0_dout;		// tmr 0 data output
  wire tmr0_ack;			// tmr 0 acknowledge
  wire tmr0_irq;			// tmr 0 interrupt request
  // tmr1
  wire tmr1_stb;			// tmr 1 strobe
  wire [31:0] tmr1_dout;		// tmr 1 data output
  wire tmr1_ack;			// tmr 1 acknowledge
  wire tmr1_irq;			// tmr 1 interrupt request
  // dsp
  wire dsp_stb;				// dsp strobe
  wire [15:0] dsp_dout;			// dsp data output
  wire dsp_ack;				// dsp acknowledge
  // kbd
  wire kbd_stb;				// kbd strobe
  wire [7:0] kbd_dout;			// kbd data output
  wire kbd_ack;				// kbd acknowledge
  wire kbd_irq;				// kbd interrupt request
  // ser0
  wire ser0_stb;			// ser 0 strobe
  wire [7:0] ser0_dout;			// ser 0 data output
  wire ser0_ack;			// ser 0 acknowledge
  wire ser0_irq_r;			// ser 0 rcv interrupt request
  wire ser0_irq_t;			// ser 0 xmt interrupt request
  // ser1
  wire ser1_stb;			// ser 1 strobe
  wire [7:0] ser1_dout;			// ser 1 data output
  wire ser1_ack;			// ser 1 acknowledge
  wire ser1_irq_r;			// ser 1 rcv interrupt request
  wire ser1_irq_t;			// ser 1 xmt interrupt request
  // dsk
  wire dsk_stb;				// dsk strobe
  wire [31:0] dsk_dout;			// dsk data output
  wire dsk_ack;				// dsk acknowledge
  wire dsk_irq;				// dsk interrupt request
  // fms
  wire fms_stb;				// fms strobe
  wire [31:0] fms_dout;			// fms data output
  wire fms_ack;				// fms acknowledge
  // dac
  wire [15:0] dac_sample_l;		// dac sample value, left
  wire [15:0] dac_sample_r;		// dac sample value, right
  wire dac_next;			// dac next sample request
  // bio
  wire bio_stb;				// bio strobe
  wire [31:0] bio_dout;			// bio data output
  wire bio_ack;				// bio acknowledge

  //--------------------------------------
  // module instances
  //--------------------------------------

  clk_rst clk_rst_1(
    .clk_in(clk_in),
    .rst_inout_n(rst_inout_n),
    .sdram_clk(sdram_clk),
    .sdram_fb(sdram_fb),
    .clk(clk),
    .clk_ok(clk_ok),
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
    .clk(clk),
    .clk_ok(clk_ok),
    .rst(rst),
    .stb(ram_stb),
    .we(bus_we),
    .addr(bus_addr[24:2]),
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
    .sdram_udqm(sdram_udqm),
    .sdram_ldqm(sdram_ldqm),
    .sdram_dq(sdram_dq[15:0])
  );

  rom rom_1(
    .clk(clk),
    .rst(rst),
    .stb(rom_stb),
    .we(bus_we),
    .addr(bus_addr[20:2]),
    .data_out(rom_dout[31:0]),
    .ack(rom_ack),
    .ce_n(flash_ce_n),
    .oe_n(flash_oe_n),
    .we_n(flash_we_n),
    .rst_n(flash_rst_n),
    .byte_n(flash_byte_n),
    .a(flash_a[19:0]),
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
    .r(vga_r[2:0]),
    .g(vga_g[2:0]),
    .b(vga_b[2:0])
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

  assign pbus_a[4:3] = 2'b00;

  dsk dsk_1(
    .clk(clk),
    .rst(rst),
    .stb(dsk_stb),
    .we(bus_we),
    .addr(bus_addr[19:2]),
    .data_in(bus_dout[31:0]),
    .data_out(dsk_dout[31:0]),
    .ack(dsk_ack),
    .irq(dsk_irq),
    .ata_d(pbus_d[15:0]),
    .ata_a(pbus_a[2:0]),
    .ata_cs0_n(ata_cs0_n),
    .ata_cs1_n(ata_cs1_n),
    .ata_dior_n(pbus_read_n),
    .ata_diow_n(pbus_write_n),
    .ata_intrq(ata_intrq),
    .ata_dmarq(ata_dmarq),
    .ata_dmack_n(ata_dmack_n),
    .ata_iordy(ata_iordy)
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

  dac dac_1(
    .clk(clk),
    .rst(rst),
    .sample_l(dac_sample_l[15:0]),
    .sample_r(dac_sample_r[15:0]),
    .next(dac_next),
    .mclk(dac_mclk),
    .sclk(dac_sclk),
    .lrck(dac_lrck),
    .sdti(dac_sdti)
  );

  assign slot1_cs_n = 1'b1;
  assign slot2_cs_n = 1'b1;
  assign ether_cs_n = 1'b1;

  bio bio_1(
    .clk(clk),
    .rst(rst),
    .stb(bio_stb),
    .we(bus_we),
    .addr(bus_addr[2]),
    .data_in(bus_dout[31:0]),
    .data_out(bio_dout[31:0]),
    .ack(bio_ack),
    .sw1_1(flash_a[19]),
    .sw1_2(flash_a[18]),
    .sw1_3(sw1_3),
    .sw1_4(sw1_4),
    .sw2_n(sw2_n),
    .sw3_n(sw3_n)
  );

  //--------------------------------------
  // address decoder
  //--------------------------------------

  // RAM: architectural limit  = 512 MB
  //      implementation limit =  32 MB
  assign ram_stb =
    (bus_stb == 1 && bus_addr[31:29] == 3'b000
                  && bus_addr[28:25] == 4'b0000) ? 1 : 0;

  // ROM: architectural limit  = 256 MB
  //      implementation limit =   2 MB
  assign rom_stb =
    (bus_stb == 1 && bus_addr[31:28] == 4'b0010
                  && bus_addr[27:21] == 7'b0000000) ? 1 : 0;

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
  assign dsk_stb =
    (i_o_stb == 1 && bus_addr[27:20] == 8'h04) ? 1 : 0;
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
    (dsk_stb == 1)  ? dsk_dout[31:0] :
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
    (dsk_stb == 1)  ? dsk_ack :
    (fms_stb == 1)  ? fms_ack :
    (bio_stb == 1)  ? bio_ack :
    0;

  //--------------------------------------
  // bus interrupt request assignments
  //--------------------------------------

  assign bus_irq[15] = tmr1_irq;
  assign bus_irq[14] = tmr0_irq;
  assign bus_irq[13] = 0;
  assign bus_irq[12] = 0;
  assign bus_irq[11] = 0;
  assign bus_irq[10] = 0;
  assign bus_irq[ 9] = 0;
  assign bus_irq[ 8] = dsk_irq;
  assign bus_irq[ 7] = 0;
  assign bus_irq[ 6] = 0;
  assign bus_irq[ 5] = 0;
  assign bus_irq[ 4] = kbd_irq;
  assign bus_irq[ 3] = ser1_irq_r;
  assign bus_irq[ 2] = ser1_irq_t;
  assign bus_irq[ 1] = ser0_irq_r;
  assign bus_irq[ 0] = ser0_irq_t;

endmodule
