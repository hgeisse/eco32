//
// dsp.v -- character display interface
//


`timescale 1ns/10ps
`default_nettype none


module dsp(clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack);
    input clk;
    input rst;
    input stb;
    input we;
    input [13:2] addr;
    input [15:0] data_in;
    output reg [15:0] data_out;
    output ack;

  integer dsp_out;		// file handle for display output

  reg [15:0] mem[0:4095];	// 32 x 128 attr/char display memory
  reg state;

  initial begin
    dsp_out = $fopen("dsp.out", "w");
  end

  always @(posedge clk) begin
    if (stb & we) begin
      mem[addr[13:2]] <= data_in[15:0];
      $fdisplay(dsp_out,
                "row = %d, col = %d, attr = 0x%h, char = 0x%h",
                addr[13:9], addr[8:2], data_in[15:8], data_in[7:0]);
    end
    if (stb & ~we) begin
      data_out[15:0] <= mem[addr[13:2]];
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      state <= 1'b0;
    end else begin
      case (state)
        1'b0:
          begin
            if (stb & ~we) begin
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

  assign ack = stb & (we | state);

endmodule
