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
    .test_step(test_step),
    .test_good(test_good),
    .test_ended(test_ended)
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
  assign led_g = test_end_seen & ~any_step_failed;
  assign led_r = test_end_seen & any_step_failed;

endmodule
