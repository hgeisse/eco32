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

  //--------------------------------------------

  reg x_valid;
  reg [13:0] x_addr;
  reg [1:0] x_offs;
  wire [31:0] x_data;
  wire [7:0] x_data_out;

  assign cache_ready_out = cache_ready_in;

  always @(posedge clk) begin
    x_valid <= cache_valid_in;
    x_addr[13:0] <= cache_addr_in[15:2];
    x_offs[1:0] <= cache_addr_in[1:0];
  end

  assign x_data[31:0] =
    { ~x_addr[1:0], x_addr[5:2], 2'b00, ~x_addr[9:6], x_addr[13:10],
      x_addr[9:6], 2'b11, ~x_addr[13:10], x_addr[1:0], ~x_addr[5:2] };

  assign x_data_out[7:0] =
    ~x_offs[1] ?
      (~x_offs[0] ? x_data[31:24] : x_data[23:16]) :
      (~x_offs[0] ? x_data[15: 8] : x_data[ 7: 0]);

  assign cache_valid_out = x_valid;
  assign cache_data_out[7:0] = x_data_out[7:0];

  //--------------------------------------------

endmodule
