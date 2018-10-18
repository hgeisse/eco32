//
// ser.v -- serial line interface
//


`timescale 1ns/10ps
`default_nettype none


`define TICKS_PER_CHAR		32'h00000200	// output speed


module ser(i,				// instance flag
           clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack, irq_r, irq_t);
    input i;
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

  wire wr_rcv_ctrl;
  wire rd_rcv_data;
  wire wr_xmt_ctrl;
  wire wr_xmt_data;

  reg [39:0] ser_data[0:255];		// space for 256 input requests
  reg [7:0] ser_data_index;		// next location to read
  wire [39:0] next_full;		// 40 bits from ser_data
  wire [31:0] next_time;		// 32 bits delta clock ticks
  wire [7:0] next_code;			// 8 bits character code
  reg [31:0] rcv_count;			// input tick counter
  wire next_rdy;			// delta time expired

  integer ser_out;			// file handle for serial output
  reg [31:0] xmt_count;			// output tick counter

  reg rcv_rdy;
  reg rcv_ien;
  reg [7:0] rcv_data;
  reg xmt_rdy;
  reg xmt_ien;
  reg [7:0] xmt_data;

  assign wr_rcv_ctrl =
    (stb == 1'b1 && we == 1'b1 && addr == 2'b00) ? 1'b1 : 1'b0;
  assign rd_rcv_data =
    (stb == 1'b1 && we == 1'b0 && addr == 2'b01) ? 1'b1 : 1'b0;
  assign wr_xmt_ctrl =
    (stb == 1'b1 && we == 1'b1 && addr == 2'b10) ? 1'b1 : 1'b0;
  assign wr_xmt_data =
    (stb == 1'b1 && we == 1'b1 && addr == 2'b11) ? 1'b1 : 1'b0;

  //------------------------------------------------------------

  initial begin
    if (~i) begin
      $readmemh("ser0.dat", ser_data);
    end else begin
      $readmemh("ser1.dat", ser_data);
    end
  end

  assign next_full[39:0] = ser_data[ser_data_index];
  assign next_time[31:0] = next_full[39:8];
  assign next_code[7:0] = next_full[7:0];

  always @(posedge clk) begin
    if (rst) begin
      ser_data_index[7:0] <= 8'd0;
      rcv_count[31:0] <= 32'd0;
    end else begin
      if (rcv_count[31:0] == 32'd0) begin
        rcv_count[31:0] <= next_time[31:0];
      end else
      if (rcv_count[31:0] == 32'd1) begin
        ser_data_index[7:0] <= ser_data_index[7:0] + 8'd1;
        rcv_count[31:0] <= rcv_count[31:0] - 32'd1;
      end else begin
        if (rcv_count[31:0] != 32'hFFFFFFFF) begin
          rcv_count[31:0] <= rcv_count[31:0] - 32'd1;
        end
      end
    end
  end

  assign next_rdy = (rcv_count[31:0] == 32'd1) ? 1'b1 : 1'b0;

  always @(posedge clk) begin
    if (rst) begin
      rcv_rdy <= 1'b0;
      rcv_data[7:0] <= 8'd0;
    end else begin
      if (next_rdy) begin
        rcv_data[7:0] <= next_code[7:0];
      end
      if (next_rdy | rd_rcv_data) begin
        rcv_rdy <= next_rdy;
      end
    end
  end

  //------------------------------------------------------------

  initial begin
    if (~i) begin
      ser_out = $fopen("ser0.out", "w");
    end else begin
      ser_out = $fopen("ser1.out", "w");
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      xmt_count[31:0] <= 32'd0;
      xmt_rdy <= 1'b1;
      xmt_data[7:0] <= 8'd0;
    end else begin
      if (wr_xmt_data) begin
        xmt_count[31:0] <= `TICKS_PER_CHAR;
        xmt_rdy <= 1'b0;
        xmt_data[7:0] <= data_in[7:0];
      end else begin
        if (xmt_count[31:0] == 32'd1) begin
          xmt_count[31:0] <= xmt_count[31:0] - 32'd1;
          xmt_rdy <= 1'b1;
          $fdisplay(ser_out, "char = 0x%h", xmt_data);
        end else begin
          if (xmt_count[31:0] != 32'd0) begin
            xmt_count[31:0] <= xmt_count[31:0] - 32'd1;
          end
        end
      end
    end
  end

  //------------------------------------------------------------

  always @(posedge clk) begin
    if (rst) begin
      rcv_ien <= 1'b0;
      xmt_ien <= 1'b0;
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
        data_out[7:0] = { 6'b000000, rcv_ien, rcv_rdy };
      2'b01:
        // rcv data
        data_out[7:0] = rcv_data[7:0];
      2'b10:
        // xmt ctrl
        data_out[7:0] = { 6'b000000, xmt_ien, xmt_rdy };
      2'b11:
        // xmt data (cannot be read)
        data_out[7:0] = 8'hxx;
      default:
        data_out[7:0] = 8'hxx;
    endcase
  end

  assign ack = stb;
  assign irq_r = rcv_ien & rcv_rdy;
  assign irq_t = xmt_ien & xmt_rdy;

endmodule
