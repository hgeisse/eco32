//
// eco32.v -- ECO32 top-level description
//


`timescale 1ns/10ps
`default_nettype none


module eco32(clk_in,
             rst_in_n
            );

    // clock and reset
    input clk_in;			// clock input, 50 MHz
    input rst_in_n;			// reset input, active low

  // clk_rst
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
  // test indicators
  wire [7:0] first_fail;		// first test step that failed
  wire led_g;				// test succeeded
  wire led_r;				// test failed

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
    .clk(clk),
    .rst(rst),
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
    .data_timeout()
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
    .data_timeout()
  );

  //--------------------------------------
  // test indicators
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

  assign first_fail[7:0] = first_step_failed[7:0];
  assign led_g = (test_end_seen & ~any_step_failed) | ~test_crc_ok;
  assign led_r = (test_end_seen & any_step_failed) | ~test_crc_ok;

endmodule
