//
// eco32.v -- ECO32 top-level description
//


`timescale 1ns/10ps
`default_nettype none


module eco32(clk_in,
             rst_in_n,
             vga_hsync,
             vga_vsync,
             vga_clk,
             vga_sync_n,
             vga_blank_n,
             vga_r,
             vga_g,
             vga_b,
             rs232_0_rxd,
             rs232_0_txd,
             rs232_1_rxd,
             rs232_1_txd,
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
    // VGA display
    output vga_hsync;
    output vga_vsync;
    output vga_clk;
    output vga_sync_n;
    output vga_blank_n;
    output [7:0] vga_r;
    output [7:0] vga_g;
    output [7:0] vga_b;
    // serial line 0
    input rs232_0_rxd;
    output rs232_0_txd;
    // serial line 1
    input rs232_1_rxd;
    output rs232_1_txd;
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
  wire clk;				// system clock
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
  // bio

  //--------------------------------------
  // module instances
  //--------------------------------------

  clk_rst clk_rst_1(
    .clk_in(clk_in),
    .rst_in_n(rst_in_n),
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
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(bus_we),
    .addr(bus_addr[26:2]),
    .data_in(bus_dout[31:0]),
    .data_out(ram_dout[31:0]),
    .ack(ram_ack)
  );

  rom rom_1(
    .clk(clk),
    .rst(rst),
    .stb(rom_stb),
    .we(bus_we),
    .addr(bus_addr[22:2]),
    .data_out(rom_dout[31:0]),
    .ack(rom_ack)
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
    .pxclk(vga_clk),
    .sync_n(vga_sync_n),
    .blank_n(vga_blank_n),
    .r(vga_r[7:0]),
    .g(vga_g[7:0]),
    .b(vga_b[7:0])
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

  //--------------------------------------
  // address decoder
  //--------------------------------------

  // RAM: architectural limit  = 512 MB
  //      implementation limit = 128 MB
  assign rom_stb =
    (bus_stb == 1'b1 && bus_addr[31:29] == 3'b000
                     && bus_addr[28:27] == 2'b00) ? 1'b1 : 1'b0;

  // ROM: architectural limit  = 256 MB
  //      implementation limit =   8 MB
  assign ram_stb =
    (bus_stb == 1'b1 && bus_addr[31:28] == 4'b0010
                     && bus_addr[27:23] == 5'b00000) ? 1'b1 : 1'b0;

  // I/O: architectural limit  = 256 MB
  assign i_o_stb =
    (bus_stb == 1'b1 && bus_addr[31:28] == 4'b0011) ? 1'b1 : 1'b0;
  assign tmr0_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h00
                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;
  assign tmr1_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h00
                     && bus_addr[19:12] == 8'h01) ? 1'b1 : 1'b0;
  assign dsp_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h01) ? 1'b1 : 1'b0;
//  assign kbd_stb =
//    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h02) ? 1'b1 : 1'b0;
  assign ser0_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h03
                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;
  assign ser1_stb =
    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h03
                     && bus_addr[19:12] == 8'h01) ? 1'b1 : 1'b0;
//  assign dsk_stb =
//    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h04) ? 1'b1 : 1'b0;
//  assign fms_stb =
//    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h05
//                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;
//  assign bio_stb =
//    (i_o_stb == 1'b1 && bus_addr[27:20] == 8'h10
//                     && bus_addr[19:12] == 8'h00) ? 1'b1 : 1'b0;

  //--------------------------------------
  // data and acknowledge multiplexers
  //--------------------------------------

  assign bus_din[31:0] =
    (ram_stb == 1'b1)  ? ram_dout[31:0] :
    (rom_stb == 1'b1)  ? rom_dout[31:0] :
    (tmr0_stb == 1'b1) ? tmr0_dout[31:0] :
    (tmr1_stb == 1'b1) ? tmr1_dout[31:0] :
    (dsp_stb == 1'b1)  ? { 16'h0000, dsp_dout[15:0] } :
//    (kbd_stb == 1'b1)  ? { 24'h000000, kbd_dout[7:0] } :
    (ser0_stb == 1'b1) ? { 24'h000000, ser0_dout[7:0] } :
    (ser1_stb == 1'b1) ? { 24'h000000, ser1_dout[7:0] } :
//    (dsk_stb == 1'b1)  ? dsk_dout[31:0] :
//    (fms_stb == 1'b1)  ? fms_dout[31:0] :
//    (bio_stb == 1'b1)  ? bio_dout[31:0] :
    32'h00000000;

  assign bus_ack =
    (ram_stb == 1'b1)  ? ram_ack :
    (rom_stb == 1'b1)  ? rom_ack :
    (tmr0_stb == 1'b1) ? tmr0_ack :
    (tmr1_stb == 1'b1) ? tmr1_ack :
    (dsp_stb == 1'b1)  ? dsp_ack :
//    (kbd_stb == 1'b1)  ? kbd_ack :
    (ser0_stb == 1'b1) ? ser0_ack :
    (ser1_stb == 1'b1) ? ser1_ack :
//    (dsk_stb == 1'b1)  ? dsk_ack :
//    (fms_stb == 1'b1)  ? fms_ack :
//    (bio_stb == 1'b1)  ? bio_ack :
    1'b0;

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
  assign bus_irq[ 8] = 1'b0;
  assign bus_irq[ 7] = 1'b0;
  assign bus_irq[ 6] = 1'b0;
  assign bus_irq[ 5] = 1'b0;
  assign bus_irq[ 4] = 1'b0;
  assign bus_irq[ 3] = ser1_irq_r;
  assign bus_irq[ 2] = ser1_irq_t;
  assign bus_irq[ 1] = ser0_irq_r;
  assign bus_irq[ 0] = ser0_irq_t;

  //--------------------------------------
  // !!!!! TEST !!!!!
  //--------------------------------------

  reg [24:0] counter;

  always @(posedge clk) begin
    if (rst) begin
      counter <= 25'h0;
    end else begin
      counter <= counter + 25'h1;
    end
  end

  assign led_g[8] = key3_n;
  assign led_g[7] = counter[23];
  assign led_g[6] = counter[22];
  assign led_g[5] = counter[21];
  assign led_g[4] = counter[20];
  assign led_g[3] = counter[19];
  assign led_g[2] = counter[18];
  assign led_g[1] = key2_n;
  assign led_g[0] = key1_n;
  assign led_r[17] = sw[17] ? 1'b0 : counter[24];
  assign led_r[16] = sw[16] ? 1'b0 : counter[23];
  assign led_r[15] = sw[15] ? 1'b0 : counter[22];
  assign led_r[14] = sw[14] ? 1'b0 : counter[21];
  assign led_r[13] = sw[13] ? 1'b0 : counter[20];
  assign led_r[12] = sw[12] ? 1'b0 : counter[19];
  assign led_r[11] = sw[11] ? 1'b0 : counter[18];
  assign led_r[10] = sw[10] ? 1'b0 : counter[17];
  assign led_r[9] = sw[9] ? 1'b0 : counter[16];
  assign led_r[8] = sw[8] ? 1'b0 : counter[15];
  assign led_r[7] = sw[7] ? 1'b0 : counter[14];
  assign led_r[6] = sw[6] ? 1'b0 : counter[13];
  assign led_r[5] = sw[5] ? 1'b0 : counter[12];
  assign led_r[4] = sw[4] ? 1'b0 : counter[11];
  assign led_r[3] = sw[3];
  assign led_r[2] = sw[2];
  assign led_r[1] = sw[1];
  assign led_r[0] = sw[0];
  assign hex7_n[6:0] = (sw[0] == 1'b0) ? 7'b0000000 : 7'b1111111;
  assign hex6_n[6:0] = counter[22:16];
  assign hex5_n[6:0] = ~counter[24:18];
  assign hex4_n[6:0] = key3_n ? 7'b1111111 : counter[24:18];
  assign hex3_n[6:0] = ~counter[22:16];
  assign hex2_n[6:0] = key2_n ? 7'b1111111 : counter[22:16];
  assign hex1_n[6:0] = ~counter[24:18];
  assign hex0_n[6:0] = key1_n ? 7'b1111111 : counter[24:18];

endmodule
