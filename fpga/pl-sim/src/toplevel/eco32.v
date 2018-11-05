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
  wire [31:0] temp_out;			// temporary output
  wire temp_trg;			// temporary trigger
  // temporary indicators
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
    .temp_out(temp_out[31:0]),
    .temp_trg(temp_trg)
  );

  //--------------------------------------
  // temporary indicators
  //--------------------------------------

  reg led_gn;				// test succeeded
  reg led_rd;				// test failed

  always @(posedge clk) begin
    if (rst) begin
      led_gn <= 1'b0;
      led_rd <= 1'b0;
    end else begin
      if (temp_trg) begin
        if (temp_out[31:0] == 32'h00004038) begin
          led_gn <= 1'b1;
          led_rd <= 1'b0;
        end else begin
          led_gn <= 1'b0;
          led_rd <= 1'b1;
        end
      end
    end
  end

  assign led_g = led_gn;
  assign led_r = led_rd;

endmodule
