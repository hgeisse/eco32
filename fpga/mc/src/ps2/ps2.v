//
// ps2.v -- PS/2 interface
//


`timescale 1ns/10ps
`default_nettype none


module ps2(clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack, irq,
           ps2_clk, ps2_data);
    // internal interface
    input clk;
    input rst;
    input stb;
    input we;
    input addr;
    input [7:0] data_in;
    output [7:0] data_out;
    output ack;
    output irq;
    // external interface
    inout ps2_clk;
    inout ps2_data;

  //
  // decoder
  //

  wire ctrl_wr;
  wire data_rd;
  wire data_wr;

  assign ctrl_wr = stb &  we & ~addr;
  assign data_rd = stb & ~we &  addr;
  assign data_wr = stb &  we &  addr;

  //
  // interrupt control
  //

  reg rcv_ien;
  reg xmt_ien;

  always @(posedge clk) begin
    if (rst) begin
      rcv_ien <= 1'b0;
      xmt_ien <= 1'b0;
    end else begin
      if (ctrl_wr) begin
        rcv_ien <= data_in[1];
        xmt_ien <= data_in[5];
      end
    end
  end

  //
  // PS/2 protocol engine and buffer control
  //

  wire [7:0] rcv_data;
  wire rcv_error;
  wire rcv_strobe;
  reg [7:0] rcv_buf;
  reg rcv_err;
  reg rcv_rdy;

  wire xmt_ready;
  reg [7:0] xmt_buf;
  reg xmt_state;
  reg xmt_next;
  reg xmt_wrbuf;
  reg xmt_strobe;
  reg xmt_rdy;

  ps2_ctrl ps2_ctrl_0(
    .clk(clk),
    .rst(rst),
    .rcv_data(rcv_data[7:0]),
    .rcv_error(rcv_error),
    .rcv_strobe(rcv_strobe),
    .xmt_ready(xmt_ready),
    .xmt_data(xmt_buf[7:0]),
    .xmt_strobe(xmt_strobe),
    .ps2_clock(ps2_clk),
    .ps2_data(ps2_data)
  );

  always @(posedge clk) begin
    if (rst) begin
      rcv_buf[7:0] <= 8'h00;
      rcv_err <= 1'b0;
      rcv_rdy <= 1'b0;
    end else begin
      if (rcv_strobe) begin
        rcv_buf[7:0] <= rcv_data[7:0];
        rcv_err <= rcv_error;
      end
      if (rcv_strobe | data_rd) begin
        rcv_rdy <= rcv_strobe;
      end
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      xmt_state <= 1'b0;
      xmt_buf[7:0] <= 8'h00;
    end else begin
      xmt_state <= xmt_next;
      if (xmt_wrbuf) begin
        xmt_buf[7:0] <= data_in[7:0];
      end
    end
  end

  always @(*) begin
    case (xmt_state)
      1'b0:
        // xmt buffer empty
        begin
          if (~data_wr) begin
            xmt_next = 1'b0;
            xmt_wrbuf = 1'b0;
            xmt_strobe = 1'b0;
            xmt_rdy = 1'b1;
          end else begin
            xmt_next = 1'b1;
            xmt_wrbuf = 1'b1;
            xmt_strobe = 1'b0;
            xmt_rdy = 1'b0;
          end
        end
      1'b1:
        // xmt buffer full
        begin
          if (~xmt_ready) begin
            xmt_next = 1'b1;
            xmt_wrbuf = 1'b0;
            xmt_strobe = 1'b0;
            xmt_rdy = 1'b0;
          end else begin
            if (~data_wr) begin
              xmt_next = 1'b0;
              xmt_wrbuf = 1'b0;
              xmt_strobe = 1'b1;
              xmt_rdy = 1'b0;
            end else begin
              xmt_next = 1'b1;
              xmt_wrbuf = 1'b1;
              xmt_strobe = 1'b1;
              xmt_rdy = 1'b0;
            end
          end
        end
    endcase
  end

  //
  // bus interface
  //

  assign data_out[7:0] =
    ~addr ? { 1'b0, 1'b0, xmt_ien, xmt_rdy,
              1'b0, rcv_err, rcv_ien, rcv_rdy } :
            rcv_buf[7:0];
  assign ack = stb;
  assign irq = (rcv_ien & rcv_rdy) | (xmt_ien & xmt_rdy);

endmodule
