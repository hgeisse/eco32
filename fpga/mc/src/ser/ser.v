//
// ser.v -- serial line interface
//


`timescale 1ns/10ps
`default_nettype none


module ser(clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack, irq_r, irq_t,
	   rxd, txd);
    // internal interface
    input clk;
    input rst;
    input stb;
    input we;
    input [3:2] addr;
    input [7:0] data_in;
    output reg [7:0] data_out;
    output ack;
    output irq_r;
    output irq_t;
    // external interface
    input rxd;
    output txd;

  wire wr_rcv_ctrl;
  wire rd_rcv_data;
  wire wr_xmt_ctrl;
  wire wr_xmt_data;

  wire rcv_rdy;
  reg rcv_ien;
  wire [7:0] rcv_data;
  wire xmt_rdy;
  reg xmt_ien;

  assign wr_rcv_ctrl = (stb == 1 && we == 1 && addr == 2'b00) ? 1 : 0;
  assign rd_rcv_data = (stb == 1 && we == 0 && addr == 2'b01) ? 1 : 0;
  assign wr_xmt_ctrl = (stb == 1 && we == 1 && addr == 2'b10) ? 1 : 0;
  assign wr_xmt_data = (stb == 1 && we == 1 && addr == 2'b11) ? 1 : 0;

  rcvbuf rcvbuf_1(clk, rst, rd_rcv_data, rcv_rdy, rcv_data, rxd);
  xmtbuf xmtbuf_1(clk, rst, wr_xmt_data, xmt_rdy, data_in, txd);

  always @(posedge clk) begin
    if (rst) begin
      rcv_ien <= 0;
      xmt_ien <= 0;
    end else begin
      if (wr_rcv_ctrl) begin
        rcv_ien <= data_in[1];
      end
      if (wr_xmt_ctrl) begin
        xmt_ien <= data_in[1];
      end
    end
  end

  always @(*) begin
    case (addr[3:2])
      2'b00:
        // rcv ctrl
        data_out = { 6'b000000, rcv_ien, rcv_rdy };
      2'b01:
        // rcv data
        data_out = rcv_data;
      2'b10:
        // xmt ctrl
        data_out = { 6'b000000, xmt_ien, xmt_rdy };
      2'b11:
        // xmt data (cannot be read)
        data_out = 8'hxx;
      default:
        data_out = 8'hxx;
    endcase
  end

  assign ack = stb;
  assign irq_r = rcv_ien & rcv_rdy;
  assign irq_t = xmt_ien & xmt_rdy;

endmodule
