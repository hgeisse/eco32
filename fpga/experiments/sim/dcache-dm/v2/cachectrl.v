//
// cachectrl.v -- cache controller
//


`timescale 1ns/10ps
`default_nettype none


module cachectrl(clk, rst,
                 cache_ready_out, cache_valid_in, cache_rd_in,
                 cache_wr_in, cache_addr_in, cache_data_in,
                 cache_ready_in, cache_valid_out, cache_data_out,
                 memory_stb, memory_we, memory_addr,
                 memory_din, memory_dout, memory_ack);
    input clk;
    input rst;
    //----------------
    output cache_ready_out;
    input cache_valid_in;
    input cache_rd_in;
    input cache_wr_in;
    input [15:0] cache_addr_in;
    input [7:0] cache_data_in;
    //----------------
    input cache_ready_in;
    output cache_valid_out;
    output [7:0] cache_data_out;
    //----------------
    output memory_stb;
    output memory_we;
    output [13:0] memory_addr;
    output [31:0] memory_din;
    input [31:0] memory_dout;
    input memory_ack;

  reg valid_buf;

  reg wr_buf;

  wire [7:0] tag;
  wire [5:0] index;
  wire [1:0] offset;

  reg [7:0] tag_buf;
  reg [5:0] index_buf;
  reg [1:0] offset_buf;

  reg [7:0] data_buf;

  reg line_valid[0:63];
  reg line_valid_out;
  reg line_dirty[0:63];
  reg line_dirty_out;
  reg [7:0] line_tag[0:63];
  reg [7:0] line_tag_out;
  reg [31:0] line_data[0:63];
  reg [31:0] line_data_out;

  wire [31:0] line_data_mod;

  wire hit;

  //--------------------------------------------

  assign cache_ready_out = cache_ready_in & (hit | ~valid_buf);

  always @(posedge clk) begin
    if (rst) begin
      valid_buf <= 1'b0;
    end else begin
      if (cache_ready_out) begin
        valid_buf <= cache_valid_in;
      end
    end
  end

  assign cache_valid_out = valid_buf & hit;

  //--------------------------------------------

  always @(posedge clk) begin
    if (cache_ready_out) begin
      wr_buf <= cache_valid_in & cache_wr_in;
    end
  end

  assign tag[7:0] = cache_addr_in[15:8];
  assign index[5:0] = cache_addr_in[7:2];
  assign offset[1:0] = cache_addr_in[1:0];

  always @(posedge clk) begin
    if (cache_ready_out) begin
      tag_buf[7:0] <= tag[7:0];
      index_buf[5:0] <= index[5:0];
      offset_buf[1:0] <= offset[1:0];
    end
  end

  always @(posedge clk) begin
    if (cache_ready_out) begin
      data_buf[7:0] <= cache_data_in[7:0];
    end
  end

  //
  // Note: If read address = write address in the same clock cycle,
  //       the memory must read the new value that just gets written
  //       to the common address (and not the old contents).
  //       This is modeled here with blocking assignments. Check
  //       that your synthesizer translates this behavior correctly!
  //
  always @(posedge clk) begin
    if (cache_ready_out) begin
      if (wr_buf) begin
        line_data[index_buf] = line_data_mod;
      end
      line_valid_out = line_valid[index];
      line_dirty_out = line_dirty[index];
      line_tag_out = line_tag[index];
      line_data_out = line_data[index];
    end
  end

  assign line_data_mod[31:24] =
    (offset_buf[1:0] == 2'b00) ? data_buf[7:0] : line_data_out[31:24];
  assign line_data_mod[23:16] =
    (offset_buf[1:0] == 2'b01) ? data_buf[7:0] : line_data_out[23:16];
  assign line_data_mod[15: 8] =
    (offset_buf[1:0] == 2'b10) ? data_buf[7:0] : line_data_out[15: 8];
  assign line_data_mod[ 7: 0] =
    (offset_buf[1:0] == 2'b11) ? data_buf[7:0] : line_data_out[ 7: 0];

  assign cache_data_out[7:0] =
    ~offset_buf[1] ?
      (~offset_buf[0] ? line_data_out[31:24] : line_data_out[23:16]) :
      (~offset_buf[0] ? line_data_out[15: 8] : line_data_out[ 7: 0]);

  //--------------------------------------------

  assign hit = line_valid_out & (line_tag_out[7:0] == tag_buf[7:0]);

  //--------------------------------------------

  reg [6:0] i;

  initial begin
    for (i = 0; i < 64; i = i + 1) begin
      line_valid[i] = 1'b0;
      line_dirty[i] = 1'b0;
    end
  end

  //--------------------------------------------

endmodule
