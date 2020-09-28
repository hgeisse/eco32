//
// dac.v -- DAC control circuit
//


`timescale 1ns/10ps
`default_nettype none


module dac(clk, rst,
           sample_l, sample_r, next,
           mclk, bclk, lrck, sdti);
    input clk;
    input rst;
    input [19:0] sample_l;
    input [19:0] sample_r;
    output next;
    output mclk;
    output bclk;
    output lrck;
    output sdti;

  reg [9:0] timing;
  reg [63:0] sr;
  wire shift;

  always @(posedge clk) begin
    if (rst) begin
      timing <= 10'h0;
    end else begin
      timing <= timing + 1;
    end
  end

  assign mclk = timing[1];
  assign bclk = timing[3];
  assign lrck = timing[9];

  assign next = (timing[9:0] == 10'h1FF) ? 1 : 0;
  assign shift = (timing[3:0] == 4'hF) ? 1 : 0;

  always @(posedge clk) begin
    if (rst) begin
      sr <= 64'h0;
    end else begin
      if (next) begin
        sr[63:52] <= 12'h000;
        sr[51:32] <= sample_l[19:0];
        sr[31:20] <= 12'h000;
        sr[19: 0] <= sample_r[19:0];
      end else begin
        if (shift) begin
          sr[63:1] <= sr[62:0];
          sr[0] <= 1'b0;
        end
      end
    end
  end

  assign sdti = sr[63];

endmodule
