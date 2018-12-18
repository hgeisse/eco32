//
// memory.v -- memory simulation
//


`timescale 1ns/10ps
`default_nettype none


`define RD_CYCLES	4'd10	// # cycles for read, range 2..15
`define WR_CYCLES	4'd8	// # cycles for write, range 2..15


module memory(clk, rst,
              stb, we, addr,
              data_in, data_out, ack);
    input clk;
    input rst;
    input stb;
    input we;
    input [13:0] addr;
    input [31:0] data_in;
    output [31:0] data_out;
    output ack;

  reg [31:0] mem[0:16383];
  reg [31:0] mem_out;
  reg [3:0] counter;

  always @(posedge clk) begin
    if (stb) begin
      if (we) begin
        mem[addr] <= data_in;
      end
      mem_out <= mem[addr];
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      counter[3:0] <= 4'h0;
    end else begin
      if (counter[3:0] == 4'h0) begin
        if (stb & ~we) begin
          counter[3:0] <= `RD_CYCLES - 4'h1;
        end
        if (stb & we) begin
          counter[3:0] <= `WR_CYCLES - 4'h1;
        end
      end else begin
        counter[3:0] <= counter[3:0] - 4'h1;
      end
    end
  end

  assign data_out[31:0] = (ack & ~we) ? mem_out[31:0] : 32'hxxxxxxxx;
  assign ack = (counter[3:0] == 4'h1);

endmodule
