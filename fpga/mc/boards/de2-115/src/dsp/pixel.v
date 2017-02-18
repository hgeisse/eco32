//
// pixel.v -- last stage in display pipeline
//


`timescale 1ns/10ps
`default_nettype none


module pixel(clk, pixclk, attcode,
             pixel, blank, hsync_in, vsync_in, blink,
             hsync, vsync, pxclk, sync_n, blank_n, r, g, b);
    input clk;
    input pixclk;
    input [7:0] attcode;
    input pixel;
    input blank;
    input hsync_in;
    input vsync_in;
    input blink;
    output reg hsync;
    output reg vsync;
    output pxclk;
    output sync_n;
    output blank_n;
    output [7:0] r;
    output [7:0] g;
    output [7:0] b;

  wire blink_bit;
  wire bg_red;
  wire bg_green;
  wire bg_blue;
  wire inten_bit;
  wire fg_red;
  wire fg_green;
  wire fg_blue;
  wire foreground;
  wire intensify;
  wire red;
  wire green;
  wire blue;

  assign blink_bit = attcode[7];
  assign bg_red = attcode[6];
  assign bg_green = attcode[5];
  assign bg_blue = attcode[4];
  assign inten_bit = attcode[3];
  assign fg_red = attcode[2];
  assign fg_green = attcode[1];
  assign fg_blue = attcode[0];

  assign foreground = pixel & ~(blink_bit & blink);
  assign intensify = foreground & inten_bit;

  assign red = (foreground ? fg_red : bg_red);
  assign green = (foreground ? fg_green : bg_green);
  assign blue = (foreground ? fg_blue : bg_blue);

  //
  // hsync and vsync are directly connected to the monitor
  //

  always @(posedge clk) begin
    if (pixclk) begin
      hsync <= hsync_in;
      vsync <= vsync_in;
    end
  end

  //
  // all other signals are passed through the registered DAC
  //

  assign pxclk = pixclk;
  assign sync_n = 1'b0;
  assign blank_n = blank;
  assign r[7] = blank & red;
  assign r[6] = blank & intensify;
  assign r[5] = blank & red & intensify;
  assign r[4] = 1'b0;
  assign r[3] = 1'b0;
  assign r[2] = 1'b0;
  assign r[1] = 1'b0;
  assign r[0] = 1'b0;
  assign g[7] = blank & green;
  assign g[6] = blank & intensify;
  assign g[5] = blank & green & intensify;
  assign g[4] = 1'b0;
  assign g[3] = 1'b0;
  assign g[2] = 1'b0;
  assign g[1] = 1'b0;
  assign g[0] = 1'b0;
  assign b[7] = blank & blue;
  assign b[6] = blank & intensify;
  assign b[5] = blank & blue & intensify;
  assign b[4] = 1'b0;
  assign b[3] = 1'b0;
  assign b[2] = 1'b0;
  assign b[1] = 1'b0;
  assign b[0] = 1'b0;

endmodule
