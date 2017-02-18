//
// chrgen.v -- character generator
//


`timescale 1ns/10ps
`default_nettype none


module chrgen(clk, pixclk,
              chrcode, chrrow, chrcol,
              pixel,
              attcode_in, blank_in, hsync_in, vsync_in, blink_in,
              attcode_out, blank_out, hsync_out, vsync_out, blink_out);
    input clk;
    input pixclk;
    input [7:0] chrcode;
    input [3:0] chrrow;
    input [2:0] chrcol;
    output reg pixel;
    input [7:0] attcode_in;
    input blank_in;
    input hsync_in;
    input vsync_in;
    input blink_in;
    output reg [7:0] attcode_out;
    output reg blank_out;
    output reg hsync_out;
    output reg vsync_out;
    output reg blink_out;

  wire [14:0] addr;
  reg chrgen_rom[0:32767];

  initial begin
    $readmemb("chrgen_rom.init", chrgen_rom);
  end

  assign addr[14:7] = chrcode[7:0];
  assign addr[6:3] = chrrow[3:0];
  assign addr[2:0] = chrcol[2:0];

  always @(posedge clk) begin
    if (pixclk) begin
      pixel <= chrgen_rom[addr];
    end
  end

  always @(posedge clk) begin
    if (pixclk) begin
      attcode_out[7:0] <= attcode_in[7:0];
      blank_out <= blank_in;
      hsync_out <= hsync_in;
      vsync_out <= vsync_in;
      blink_out <= blink_in;
    end
  end

endmodule
