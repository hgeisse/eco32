//
// eco32.v -- ECO32 top-level description
//


`timescale 1ns/10ps
`default_nettype none


module eco32(clk_in,
             rst_in_n,
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

  //--------------------------------------
  // module instances
  //--------------------------------------

  clk_rst clk_rst_1(
    .clk_in(clk_in),
    .rst_in_n(rst_in_n),
    .clk_ok(clk_ok),
    .clk_100_ps(),
    .clk_100(clk),
    .clk_50(),
    .rst(rst)
  );

  cpu cpu_1(
    .clk(clk),
    .rst(rst),
    .test_step(test_step),
    .test_good(test_good),
    .test_ended(test_ended)
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
  assign hex6_n[6:0] = 7'h7F;
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
  reg [3:0] first_step_failed;
  reg test_end_seen;

  always @(posedge clk) begin
    if (rst) begin
      any_step_failed <= 1'b0;
      first_step_failed[3:0] <= 4'h0;
      test_end_seen <= 1'b0;
    end else begin
      if (test_step & ~test_end_seen) begin
        if (~test_good) begin
          any_step_failed <= 1'b1;
        end
        if (~any_step_failed & test_good) begin
          first_step_failed[3:0] <= first_step_failed[3:0] + 4'h1;
        end
      end
      if (test_ended) begin
        test_end_seen <= 1'b1;
      end
    end
  end

  hexdrv hexdrv_7(
    .in(first_step_failed[3:0]),
    .out(hex7_n[6:0])
  );

  assign led_g[7] = test_end_seen & ~any_step_failed;
  assign led_r[0] = test_end_seen & any_step_failed;

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
