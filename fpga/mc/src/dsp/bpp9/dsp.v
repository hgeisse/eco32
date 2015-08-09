//
// dsp.v -- character display interface
//


`timescale 1ns/10ps
`default_nettype none


module dsp(clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack,
           hsync, vsync,
           r, g, b);
    // internal interface
    input clk;
    input rst;
    input stb;
    input we;
    input [13:2] addr;
    input [15:0] data_in;
    output [15:0] data_out;
    output ack;
    // external interface
    output hsync;
    output vsync;
    output [2:0] r;
    output [2:0] g;
    output [2:0] b;

  reg state;

  display display_1(
    .clk(clk),
    .dsp_row(addr[13:9]),
    .dsp_col(addr[8:2]),
    .dsp_en(stb),
    .dsp_wr(we),
    .dsp_wr_data(data_in[15:0]),
    .dsp_rd_data(data_out[15:0]),
    .hsync(hsync),
    .vsync(vsync),
    .r(r[2:0]),
    .g(g[2:0]),
    .b(b[2:0])
  );

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
