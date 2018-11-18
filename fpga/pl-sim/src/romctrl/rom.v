//
// rom.v -- simulate external ROM
//          8M x 8 bit = 8 MB
//


`timescale 1ns/10ps
`default_nettype none


//
// use this set of parameters for minimal access times
//
//`define RD_CYCLES	4'd2	// # cycles for read, min = 2

//
// use this set of parameters for realistic access times
//
`define RD_CYCLES	4'd6	// # cycles for read, min = 2


module rom(clk, rst,
           stb, addr,
           data_out, ack);
    input clk;
    input rst;
    input stb;
    input [22:0] addr;
    output reg [7:0] data_out;
    output ack;

  reg [7:0] mem[0:8388607];
  reg [3:0] counter;

  initial begin
    #0          $readmemh("rom.dat", mem);
  end

  always @(posedge clk) begin
    if (stb) begin
      // read cycle
      data_out <= mem[addr];
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      counter[3:0] <= 4'h0;
    end else begin
      if (counter[3:0] == 4'h0) begin
        if (stb) begin
          // a read may need some clock cycles
          counter[3:0] <= `RD_CYCLES - 1;
        end
      end else begin
        counter[3:0] <= counter[3:0] - 1;
      end
    end
  end

  assign ack = (counter[3:0] == 4'h1) ? 1 : 0;

endmodule
