//
// ram.v -- simulate external RAM
//          8M x 32 bit = 32 MB
//


`timescale 1ns/10ps
`default_nettype none


//
// use this set of parameters for minimal access times
//
`define RD_CYCLES	4'd2	// # cycles for read, min = 2
`define WR_CYCLES	4'd2	// # cycles for write, min = 2

//
// use this set of parameters for realistic access times
//
//`define RD_CYCLES	4'd14	// # cycles for read, min = 2
//`define WR_CYCLES	4'd6	// # cycles for write, min = 2


module ram(clk, rst,
           stb, we, addr,
           data_in, data_out, ack);
    input clk;
    input rst;
    input stb;
    input we;
    input [24:2] addr;
    input [31:0] data_in;
    output reg [31:0] data_out;
    output ack;

  reg [31:0] mem[0:8388607];
  reg [3:0] counter;

  always @(posedge clk) begin
    if (stb) begin
      if (we) begin
        // write cycle
        mem[addr] <= data_in;
      end else begin
        // read cycle
        data_out <= mem[addr];
      end
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      counter[3:0] <= 4'h0;
    end else begin
      if (counter[3:0] == 4'h0) begin
        if (stb & ~we) begin
          // a read may need some clock cycles
          counter[3:0] <= `RD_CYCLES - 1;
        end
        if (stb & we) begin
          // a write may need some clock cycles
          counter[3:0] <= `WR_CYCLES - 1;
        end
      end else begin
        counter[3:0] <= counter[3:0] - 1;
      end
    end
  end

  assign ack = (counter[3:0] == 4'h1) ? 1 : 0;

endmodule
