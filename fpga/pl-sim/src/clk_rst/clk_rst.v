//
// clk_rst.v -- clock and reset generator
//


`timescale 1ns/10ps
`default_nettype none


module clk_rst(clk_in, rst_in_n,
               clk, rst);
    input clk_in;
    input rst_in_n;
    output clk;
    output rst;

  reg clk_dbl;
  reg rst_p_n;
  reg rst_s_n;
  reg [3:0] cnt;

  always @(posedge clk_in) begin
    #0 clk_dbl = 1'b1;
    #5 clk_dbl = 1'b0;
    #5 clk_dbl = 1'b1;
    #5 clk_dbl = 1'b0;
  end

  assign clk = clk_dbl;

  always @(posedge clk) begin
    rst_p_n <= rst_in_n;
    rst_s_n <= rst_p_n;
    if (~rst_s_n) begin
      cnt <= 4'h0;
    end else begin
      if (cnt != 4'hF) begin
        cnt <= cnt + 4'h1;
      end
    end
  end

  assign rst = (cnt == 4'hF) ? 1'b0 : 1'b1;

endmodule
