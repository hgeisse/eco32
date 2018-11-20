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
    input clk_in;			// clock input, 50 MHz
    input rst_in_n;			// reset input, active low
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
  wire clk;				// system clock, 100 MHz
  wire rst;				// system reset, active high
  // cpu
  wire test_step;			// test step completed
  wire test_good;			// test step good
  wire test_ended;			// test ended
  wire test_crc_ok;			// test if CRC value is good
  // ramctrl
  wire ram_inst_stb;			// RAM inst strobe
  wire [24:0] ram_inst_addr;		// RAM inst address (cache line)
  wire [127:0] ram_inst_dout;		// RAM inst data out (cache line)
  wire ram_inst_ack;			// RAM inst acknowledge
  wire ram_inst_timeout;		// RAM inst timeout
  // romctrl
  wire rom_inst_stb;			// ROM inst strobe
  wire [23:0] rom_inst_addr;		// ROM inst address (cache line)
  wire [127:0] rom_inst_dout;		// ROM inst data out (cache line)
  wire rom_inst_ack;			// ROM inst acknowledge
  wire rom_inst_timeout;		// ROM inst timeout

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

  cpu cpu_1(
    .clk(clk),
    .rst(rst),
    //----------------
    .ram_inst_stb(ram_inst_stb),
    .ram_inst_addr(ram_inst_addr[24:0]),
    .ram_inst_dout(ram_inst_dout[127:0]),
    .ram_inst_ack(ram_inst_ack),
    .ram_inst_timeout(ram_inst_timeout),
    //----------------
    .rom_inst_stb(rom_inst_stb),
    .rom_inst_addr(rom_inst_addr[23:0]),
    .rom_inst_dout(rom_inst_dout[127:0]),
    .rom_inst_ack(rom_inst_ack),
    .rom_inst_timeout(rom_inst_timeout),
    //----------------
    .test_step(test_step),
    .test_good(test_good),
    .test_ended(test_ended),
    .test_crc_ok(test_crc_ok)
  );

  ramctrl ramctrl_1(
    .clk_ok(clk_ok),
    .clk(clk),
    .inst_stb(ram_inst_stb),
    .inst_addr(ram_inst_addr[24:0]),
    .inst_dout(ram_inst_dout[127:0]),
    .inst_ack(ram_inst_ack),
    .inst_timeout(ram_inst_timeout),
    .data_stb(1'b0),
    .data_we(1'b0),
    .data_addr(25'h0),
    .data_din(128'h0),
    .data_dout(),
    .data_ack(),
    .data_timeout(),
    //----------------
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

  romctrl romctrl_1(
    .clk(clk),
    .rst(rst),
    .inst_stb(rom_inst_stb),
    .inst_addr(rom_inst_addr[23:0]),
    .inst_dout(rom_inst_dout[127:0]),
    .inst_ack(rom_inst_ack),
    .inst_timeout(rom_inst_timeout),
    .data_stb(1'b0),
    .data_addr(24'h0),
    .data_dout(),
    .data_ack(),
    .data_timeout(),
    //----------------
    .fl_ce_n(fl_ce_n),
    .fl_oe_n(fl_oe_n),
    .fl_we_n(fl_we_n),
    .fl_wp_n(fl_wp_n),
    .fl_rst_n(fl_rst_n),
    .fl_a(fl_addr[22:0]),
    .fl_d(fl_dq[7:0])
  );

  //--------------------------------------
  // temporary external connections
  //--------------------------------------

  // board I/O
  assign led_g[8] = ~key3_n & ~key2_n & ~key1_n;
  //assign led_g[7] = 1'b0;
  assign led_g[6] = 1'b0;
  assign led_g[5] = 1'b0;
  assign led_g[4] = 1'b0;
  assign led_g[3] = 1'b0;
  //assign led_g[2] = 1'b0;
  //assign led_g[1] = 1'b0;
  //assign led_g[0] = 1'b0;
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
  //assign led_r[0] = 1'b0;
  //assign hex7_n[6:0] = 7'h7F;
  //assign hex6_n[6:0] = 7'h7F;
  assign hex5_n[6:0] = 7'h7F;
  assign hex4_n[6:0] = 7'h7F;
  assign hex3_n[6:0] = 7'h7F;
  assign hex2_n[6:0] = 7'h7F;
  assign hex1_n[6:0] = 7'h7F;
  assign hex0_n[6:0] = 7'h7F;

  //--------------------------------------
  // test indicators
  //--------------------------------------

  reg [25:0] heartbeat;

  always @(posedge clk) begin
    heartbeat[25:0] <= heartbeat[25:0] + 16'h1;
  end

  assign led_g[0] = clk_ok;
  assign led_g[1] = heartbeat[25];
  assign led_g[2] = rst;

  //--------------------------------------

  reg any_step_failed;
  reg [7:0] first_step_failed;
  reg test_end_seen;

  always @(posedge clk) begin
    if (rst) begin
      any_step_failed <= 1'b0;
      first_step_failed[7:0] <= 8'h00;
      test_end_seen <= 1'b0;
    end else begin
      if (test_step & ~test_end_seen) begin
        if (~test_good) begin
          any_step_failed <= 1'b1;
        end
        if (~any_step_failed & test_good) begin
          first_step_failed[7:0] <= first_step_failed[7:0] + 8'h01;
        end
      end
      if (test_ended) begin
        test_end_seen <= 1'b1;
      end
    end
  end

  hexdrv hexdrv_7(
    .in(first_step_failed[7:4]),
    .out(hex7_n[6:0])
  );

  hexdrv hexdrv_6(
    .in(first_step_failed[3:0]),
    .out(hex6_n[6:0])
  );

  assign led_g[7] = (test_end_seen & ~any_step_failed) | ~test_crc_ok;
  assign led_r[0] = (test_end_seen & any_step_failed) | ~test_crc_ok;

endmodule


module hexdrv(in, out);
    input [3:0] in;
    output reg [6:0] out;

  always @(*) begin
    case (in[3:0])
                        // 6543210
      4'h0: out[6:0] = ~7'b0111111;
      4'h1: out[6:0] = ~7'b0000110;
      4'h2: out[6:0] = ~7'b1011011;
      4'h3: out[6:0] = ~7'b1001111;
      4'h4: out[6:0] = ~7'b1100110;
      4'h5: out[6:0] = ~7'b1101101;
      4'h6: out[6:0] = ~7'b1111101;
      4'h7: out[6:0] = ~7'b0000111;
      4'h8: out[6:0] = ~7'b1111111;
      4'h9: out[6:0] = ~7'b1100111;
      4'hA: out[6:0] = ~7'b1110111;
      4'hB: out[6:0] = ~7'b1111100;
      4'hC: out[6:0] = ~7'b0111001;
      4'hD: out[6:0] = ~7'b1011110;
      4'hE: out[6:0] = ~7'b1111001;
      4'hF: out[6:0] = ~7'b1110001;
    endcase
  end

endmodule
