//
// eco32.v -- ECO32 top-level description
//


`timescale 1ns/10ps
`default_nettype none


module eco32(clk_in, rst_in_n);
    input clk_in;			// clock input
    input rst_in_n;			// reset input

  // clk_rst
  wire clk;				// system clock
  wire rst;				// system reset
  // cpu
  wire bus_stb;				// bus strobe
  wire bus_we;				// bus write enable
  wire [31:2] bus_addr;			// bus address (word address)
  wire [31:0] bus_addr32;		// bus address (byte address)
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

  // show the full 32-bit address in the simulation results
  assign bus_addr32[31:0] = { bus_addr[31:2], 2'b00 };

  ram ram_1(
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(bus_we),
    .addr(bus_addr[24:2]),
    .data_in(bus_dout[31:0]),
    .data_out(ram_dout[31:0]),
    .ack(ram_ack)
  );

  rom rom_1(
    .clk(clk),
    .rst(rst),
    .stb(rom_stb),
    .we(bus_we),
    .addr(bus_addr[15:2]),
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
    .ack(dsp_ack)
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
    .irq(kbd_irq)
  );

  ser ser_1(
    .i(1'b0),
    .clk(clk),
    .rst(rst),
    .stb(ser0_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[7:0]),
    .data_out(ser0_dout[7:0]),
    .ack(ser0_ack),
    .irq_r(ser0_irq_r),
    .irq_t(ser0_irq_t)
  );

  ser ser_2(
    .i(1'b1),
    .clk(clk),
    .rst(rst),
    .stb(ser1_stb),
    .we(bus_we),
    .addr(bus_addr[3:2]),
    .data_in(bus_dout[7:0]),
    .data_out(ser1_dout[7:0]),
    .ack(ser1_ack),
    .irq_r(ser1_irq_r),
    .irq_t(ser1_irq_t)
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
  //      implementation limit =  64 KB
  assign rom_stb =
    (bus_stb == 1 && bus_addr[31:28] == 4'b0010
                  && bus_addr[27:16] == 12'b000000000000) ? 1 : 0;

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
  assign bus_irq[ 8] = 0;
  assign bus_irq[ 7] = 0;
  assign bus_irq[ 6] = 0;
  assign bus_irq[ 5] = 0;
  assign bus_irq[ 4] = kbd_irq;
  assign bus_irq[ 3] = ser1_irq_r;
  assign bus_irq[ 2] = ser1_irq_t;
  assign bus_irq[ 1] = ser0_irq_r;
  assign bus_irq[ 0] = ser0_irq_t;

endmodule
