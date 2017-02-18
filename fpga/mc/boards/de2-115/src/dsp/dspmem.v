//
// dspmem.v -- display memory
//


`timescale 1ns/10ps
`default_nettype none


module dspmem(rdwr_row, rdwr_col, wr_data, rd_data, en, wr,
              clk, pixclk,
              txtrow, txtcol, attcode, chrcode,
              chrrow_in, chrcol_in, blank_in,
              hsync_in, vsync_in, blink_in,
              chrrow_out, chrcol_out, blank_out,
              hsync_out, vsync_out, blink_out);
    input [4:0] rdwr_row;
    input [6:0] rdwr_col;
    input [15:0] wr_data;
    output [15:0] rd_data;
    input en;
    input wr;
    input clk;
    input pixclk;
    input [4:0] txtrow;
    input [6:0] txtcol;
    output [7:0] attcode;
    output [7:0] chrcode;
    input [3:0] chrrow_in;
    input [2:0] chrcol_in;
    input blank_in;
    input hsync_in;
    input vsync_in;
    input blink_in;
    output reg [3:0] chrrow_out;
    output reg [2:0] chrcol_out;
    output reg blank_out;
    output reg hsync_out;
    output reg vsync_out;
    output reg blink_out;

  wire [11:0] rdwr_addr;
  wire [7:0] rdwr_din_att;
  reg [7:0] rdwr_dout_att;
  wire [7:0] rdwr_din_chr;
  reg [7:0] rdwr_dout_chr;

  wire [11:0] rfsh_addr;
  reg [7:0] rfsh_dout_att;
  reg [7:0] rfsh_dout_chr;

  reg [7:0] dspmem_att[0:4095];
  reg [7:0] dspmem_chr[0:4095];

  initial begin
    $readmemh("dspmem_att.init", dspmem_att);
    $readmemh("dspmem_chr.init", dspmem_chr);
  end

  assign rdwr_addr[11:7] = rdwr_row[4:0];
  assign rdwr_addr[6:0] = rdwr_col[6:0];
  assign rdwr_din_att[7:0] = wr_data[15:8];
  assign rdwr_din_chr[7:0] = wr_data[7:0];
  assign rd_data[15:8] = rdwr_dout_att[7:0];
  assign rd_data[7:0] = rdwr_dout_chr[7:0];

  assign rfsh_addr[11:7] = txtrow[4:0];
  assign rfsh_addr[6:0] = txtcol[6:0];
  assign attcode[7:0] = rfsh_dout_att[7:0];
  assign chrcode[7:0] = rfsh_dout_chr[7:0];

  always @(posedge clk) begin
    /* read/write port */
    if (en) begin
      if (wr) begin
        dspmem_att[rdwr_addr] <= rdwr_din_att;
      end
      rdwr_dout_att <= dspmem_att[rdwr_addr];
    end
    /* refresh port */
    if (pixclk) begin
      rfsh_dout_att <= dspmem_att[rfsh_addr];
    end
  end

  always @(posedge clk) begin
    /* read/write port */
    if (en) begin
      if (wr) begin
        dspmem_chr[rdwr_addr] <= rdwr_din_chr;
      end
      rdwr_dout_chr <= dspmem_chr[rdwr_addr];
    end
    /* refresh port */
    if (pixclk) begin
      rfsh_dout_chr <= dspmem_chr[rfsh_addr];
    end
  end

  always @(posedge clk) begin
    if (pixclk) begin
      chrrow_out[3:0] <= chrrow_in[3:0];
      chrcol_out[2:0] <= chrcol_in[2:0];
      blank_out <= blank_in;
      hsync_out <= hsync_in;
      vsync_out <= vsync_in;
      blink_out <= blink_in;
    end
  end

endmodule
