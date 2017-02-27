//
// ram.v -- external RAM interface, using SDRAM
//          32M x 32 bit = 128 MB
//


`timescale 1ns/10ps
`default_nettype none


module ram(clk, rst,
           stb, we, addr,
           data_in, data_out, ack);
    // internal interface signals
    input clk;
    input rst;
    input stb;
    input we;
    input [26:2] addr;
    input [31:0] data_in;
    output reg [31:0] data_out;
    output ack;

  wire en;
  wire [13:0] ad;
  reg [31:0] ram[0:16383];
  reg state;

  assign en = stb & ~(|addr[26:16]);
  assign ad[13:0] = addr[15:2];

  always @(posedge clk) begin
    if (en) begin
      if (we) begin
        ram[ad] <= data_in;
      end
      data_out <= ram[ad];
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      state <= 1'b0;
    end else begin
      case (state)
        1'b0:
          begin
            if (en & ~we) begin
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

  assign ack = en & (we | state);

endmodule
