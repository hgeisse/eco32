//
// cachectrl.v -- cache controller
//


`timescale 1ns/10ps
`default_nettype none


module cachectrl(clk, rst,
                 cache_ready_out, cache_valid_in,
                 cache_rd_in, cache_wr_in, cache_addr_in, cache_data_in,
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

  //--------------------------------------------

  assign cache_ready_out = cache_ready_in;

  always @(posedge clk) begin
    if (rst) begin
      valid_buf <= 1'b0;
    end else begin
      if (cache_ready_out) begin
        valid_buf <= cache_valid_in;
      end
    end
  end

  assign cache_valid_out = valid_buf;

  //--------------------------------------------

  wire ram_en;
  wire ram_rd;
  wire ram_wr;
  wire [15:0] ram_addr;
  wire [7:0] ram_data_in;
  reg [7:0] ram[0:65535];
  reg [7:0] ram_data_out;

  assign ram_en = cache_valid_in;
  assign ram_rd = cache_rd_in;
  assign ram_wr = cache_wr_in;
  assign ram_addr[15:0] = cache_addr_in[15:0];
  assign ram_data_in[7:0] = cache_data_in[7:0];

  always @(posedge clk) begin
    if (ram_en) begin
      if (ram_wr) begin
        ram[ram_addr] <= ram_data_in;
      end
      ram_data_out <= ram_rd ? ram[ram_addr] : ram_data_in;
//      if (ram_wr) begin
//        $display("time %t: write, addr = 0x%x, data = 0x%x",
//                 $time, ram_addr, ram_data_in);
//      end
//      if (ram_rd) begin
//        $display("time %t: read, addr = 0x%x, data = 0x%x",
//                 $time, ram_addr, ram_data_out);
//      end
    end
  end

  assign cache_data_out[7:0] =
    cache_valid_out ? ram_data_out[7:0] : 8'hxx;

  //--------------------------------------------

endmodule
