//
// memory.v -- memory simulation
//


`timescale 1ns/10ps
`default_nettype none


`define RD_CYCLES	4'd10	// # cycles for read, range 2..15


module memory(clk, rst,
              stb, addr,
              data, ack);
    input clk;
    input rst;
    input stb;
    input [13:0] addr;
    output [31:0] data;
    output ack;

  reg [3:0] counter;
  wire [31:0] fake_data;

  always @(posedge clk) begin
    if (rst) begin
      counter[3:0] <= 4'h0;
    end else begin
      if (counter[3:0] == 4'h0) begin
        if (stb) begin
          counter[3:0] <= `RD_CYCLES - 4'h1;
        end
      end else begin
        counter[3:0] <= counter[3:0] - 4'h1;
      end
    end
  end

  assign fake_data[31:0] =
    { ~addr[1:0], addr[5:2], 2'b00, ~addr[9:6], addr[13:10],
      addr[9:6], 2'b11, ~addr[13:10], addr[1:0], ~addr[5:2] };

  assign data[31:0] =
    (counter[3:0] == 4'h1) ? fake_data[31:0] : 32'hxxxxxxxx;
  assign ack =
    (counter[3:0] == 4'h1) ? 1'b1 : 1'b0;

endmodule
