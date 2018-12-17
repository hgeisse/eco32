//
// cachectrl.v -- cache controller
//


`timescale 1ns/10ps
`default_nettype none


module cachectrl(clk, rst,
                 cache_ready_out, cache_valid_in, cache_addr_in,
                 cache_ready_in, cache_valid_out, cache_data_out,
                 memory_stb, memory_addr, memory_data, memory_ack);
    input clk;
    input rst;
    //----------------
    output cache_ready_out;
    input cache_valid_in;
    input [15:0] cache_addr_in;
    //----------------
    input cache_ready_in;
    output cache_valid_out;
    output [7:0] cache_data_out;
    //----------------
    output memory_stb;
    output [13:0] memory_addr;
    input [31:0] memory_data;
    input memory_ack;

  reg valid_buf;

  wire [7:0] tag;
  wire [5:0] index;
  wire [1:0] offset;

  reg [7:0] tag_buf;
  reg [5:0] index_buf;
  reg [1:0] offset_buf;

  wire [1:0] sel_index;
  reg [5:0] cur_index;

  reg line_valid[0:63];
  reg line_valid_out;
  reg [7:0] line_tag[0:63];
  reg [7:0] line_tag_out;
  reg [31:0] line_data[0:63];
  reg [31:0] line_data_out;

  wire hit;

  reg memory_ack_buf;

  //--------------------------------------------

  assign cache_ready_out =
    cache_ready_in & (hit | ~valid_buf) & ~invalidate;

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

  always @(*) begin
    case (sel_index[1:0])
      2'b00: cur_index[5:0] = index[5:0];
      2'b01: cur_index[5:0] = index_buf[5:0];
      2'b10: cur_index[5:0] = index_count[5:0];
      2'b11: cur_index[5:0] = index_count[5:0];
    endcase
  end

  always @(posedge clk) begin
    if (cache_ready_out | memory_ack_buf) begin
      line_valid_out <= line_valid[cur_index];
      line_tag_out <= line_tag[cur_index];
      line_data_out <= line_data[cur_index];
    end
    if (memory_ack | invalidate) begin
      line_valid[cur_index] <= ~invalidate;
      line_tag[cur_index] <= tag_buf;
      line_data[cur_index] <= memory_data;
    end
  end

  assign cache_data_out[7:0] =
    ~offset_buf[1] ?
      (~offset_buf[0] ? line_data_out[31:24] : line_data_out[23:16]) :
      (~offset_buf[0] ? line_data_out[15: 8] : line_data_out[ 7: 0]);

  //--------------------------------------------

  assign hit = line_valid_out & (line_tag_out[7:0] == tag_buf[7:0]);

  assign memory_stb = ~hit & valid_buf & ~memory_ack_buf;
  assign memory_addr[13:0] = { tag_buf[7:0], index_buf[5:0] };

  always @(posedge clk) begin
    memory_ack_buf <= memory_ack;
  end

  assign sel_index = { invalidate, memory_ack | memory_ack_buf };

  //--------------------------------------------

  reg [6:0] index_count;
  wire invalidate;

  always @(posedge clk) begin
    if (rst) begin
      index_count[6:0] <= 7'd0;
    end else begin
      if (~index_count[6]) begin
        index_count[6:0] <= index_count[6:0] + 7'd1;
      end
    end
  end

  assign invalidate = ~index_count[6];

endmodule
