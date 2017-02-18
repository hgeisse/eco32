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
  wire bus_we;				// bus write enable
  wire [31:2] bus_addr;			// bus address (word address)
  wire [31:0] bus_dout;			// bus data output, for writes
  // ram
  // rom
  // i/o
  // tmr0
  // tmr1
  // dsp
  wire dsp_stb;				// dsp strobe
  wire [15:0] dsp_dout;			// dsp data output
  wire dsp_ack;				// dsp acknowledge
  // kbd
  // ser0
  // ser1
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

  //--------------------------------------
  // address decoder
  //--------------------------------------

  //--------------------------------------
  // data and acknowledge multiplexers
  //--------------------------------------

  //--------------------------------------
  // bus interrupt request assignments
  //--------------------------------------

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

  //-------------------

  wire en;
  wire wr;
  wire [4:0] rdwr_row;
  wire [6:0] rdwr_col;
  wire [7:0] wr_data;

  reg [17:0] prescaler;
  reg [11:0] position;

  always @(posedge clk) begin
    prescaler <= prescaler + 18'h1;
  end

  assign en = (prescaler == 18'h3FFFF) &&
              (position != 12'hFFF);

  always @(posedge clk) begin
    if (en) begin
      position <= position + 12'h1;
    end
  end

  assign wr = en;
  assign rdwr_row[4:0] = position[11:7];
  assign rdwr_col[6:0] = position[6:0];
  assign wr_data =
    (rdwr_row == 5'd14 && rdwr_col == 7'd36) ? 8'h54 :
    (rdwr_row == 5'd14 && rdwr_col == 7'd38) ? 8'h65 :
    (rdwr_row == 5'd14 && rdwr_col == 7'd40) ? 8'h73 :
    (rdwr_row == 5'd14 && rdwr_col == 7'd42) ? 8'h74 :
                                               8'h20;

  assign dsp_stb = en;
  assign bus_we = wr;
  assign bus_addr[13:2] = { rdwr_row[4:0], rdwr_col[6:0] };
  assign bus_dout[15:0] = { 8'h07, wr_data[7:0] };

  //-------------------

endmodule
