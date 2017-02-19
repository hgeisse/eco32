//
// rom.v -- Flash ROM interface
//          2M x 32 bit = 8 MB
//


`timescale 1ns/10ps
`default_nettype none


module rom(clk, rst,
           stb, we, addr,
           data_out, ack);
    // internal interface signals
    input clk;
    input rst;
    input stb;
    input we;
    input [22:2] addr;
    output reg [31:0] data_out;
    output ack;

  wire en;
  wire [9:0] ad;
  reg [31:0] rom[0:1023];
  reg state;

  assign en = stb & ~we & ~(|addr[22:12]);
  assign ad[9:0] = addr[11:2];

  always @(posedge clk) begin
    if (en) begin
      data_out <= rom[ad];
    end
  end

  initial begin
    $readmemh("rom.init", rom);
  end

  always @(posedge clk) begin
    if (rst) begin
      state <= 1'b0;
    end else begin
      case (state)
        1'b0:
          begin
            if (en) begin
              state <= 1'b1;
            end
          end
        1'b1:
          begin
            state <= 1'b0;
          end
      endcase
    end
  end

  assign ack = en & state;

endmodule
