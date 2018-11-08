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
  // test indicators
  wire [3:0] step_failed;		// first test step that failed
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
    .test_step(test_step),
    .test_good(test_good),
    .test_ended(test_ended)
  );

  //--------------------------------------
  // test indicators
  //--------------------------------------

  reg any_step_failed;
  reg [3:0] first_step_failed;

  always @(posedge clk) begin
    if (rst) begin
      any_step_failed <= 1'b0;
      first_step_failed[3:0] <= 4'h0;
    end else begin
      if (test_step) begin
        if (~test_good) begin
          any_step_failed <= 1'b1;
        end
        if (~any_step_failed & test_good) begin
          first_step_failed[3:0] <= first_step_failed[3:0] + 4'h1;
        end
      end
    end
  end

  assign step_failed[3:0] = first_step_failed[3:0];

  reg led_gn;
  reg led_rd;

  always @(posedge clk) begin
    if (rst) begin
      led_gn <= 1'b0;
      led_rd <= 1'b0;
    end else begin
      if (test_ended & ~led_gn & ~led_rd) begin
        if (any_step_failed) begin
          led_gn <= 1'b0;
          led_rd <= 1'b1;
        end else begin
          led_gn <= 1'b1;
          led_rd <= 1'b0;
        end
      end
    end
  end

  assign led_g = led_gn;
  assign led_r = led_rd;

endmodule
