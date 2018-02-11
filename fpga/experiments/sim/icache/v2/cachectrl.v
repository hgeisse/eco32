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

  reg line_valid[0:63];
  reg line_valid_out;
  reg [7:0] line_tag[0:63];
  reg [7:0] line_tag_out;
  reg [31:0] line_data[0:63];
  reg [31:0] line_data_out;

  reg [7:0] tag_buf;
  reg [5:0] index_buf;
  reg [1:0] offset_buf;

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

  assign tag[7:0] = cache_addr_in[15:8];
  assign index[5:0] = cache_addr_in[7:2];
  assign offset[1:0] = cache_addr_in[1:0];

  always @(posedge clk) begin
    if (cache_ready_out) begin
      line_valid_out <= line_valid[index];
      line_tag_out <= line_tag[index];
      line_data_out <= line_data[index];
    end
  end

  always @(posedge clk) begin
    if (cache_ready_out) begin
      tag_buf[7:0] <= tag[7:0];
      index_buf[5:0] <= index[5:0];
      offset_buf[1:0] <= offset[1:0];
    end
  end

  assign cache_data_out[7:0] =
    ~offset_buf[1] ?
      (~offset_buf[0] ? line_data_out[31:24] : line_data_out[23:16]) :
      (~offset_buf[0] ? line_data_out[15: 8] : line_data_out[ 7: 0]);

  //--------------------------------------------

  assign hit = line_valid_out & (line_tag_out[7:0] == tag_buf[7:0]);

  //--------------------------------------------

  initial begin
    #705        line_valid[0] = 1'b1;
                line_tag[0] = 8'h00;
                line_data[0] = 32'hC0F00FCF;
                line_valid[1] = 1'b1;
                line_tag[1] = 8'h00;
                line_data[1] = 32'h80F00FDF;
                line_valid[2] = 1'b0;
                line_tag[2] = 8'hA2;
                line_data[2] = 32'h12345676;
                line_valid[3] = 1'b0;
                line_tag[3] = 8'hA3;
                line_data[3] = 32'h12345675;
                line_valid[4] = 1'b0;
                line_tag[4] = 8'hA4;
                line_data[4] = 32'h12345674;
                line_valid[5] = 1'b0;
                line_tag[5] = 8'hA5;
                line_data[5] = 32'h12345673;
                line_valid[6] = 1'b0;
                line_tag[6] = 8'hA6;
                line_data[6] = 32'h12345672;
                line_valid[7] = 1'b0;
                line_tag[7] = 8'hA7;
                line_data[7] = 32'h12345671;
  end

  //--------------------------------------------

endmodule
