//
// ramctrl.v -- RAM controller
//


`include "ramctrl/ram.v"


`timescale 1ns/10ps
`default_nettype none


module ramctrl(clk, rst,
               inst_stb, inst_addr,
               inst_dout, inst_ack,
               inst_timeout,
               data_stb, data_we,
               data_addr, data_din,
               data_dout, data_ack,
               data_timeout);
    input clk;
    input rst;
    input inst_stb;
    input [24:0] inst_addr;
    output [127:0] inst_dout;
    output reg inst_ack;
    output reg inst_timeout;
    input data_stb;
    input data_we;
    input [24:0] data_addr;
    input [127:0] data_din;
    output [127:0] data_dout;
    output reg data_ack;
    output reg data_timeout;

  reg ram_stb;
  reg ram_we;
  wire [24:0] ram_addr;
  reg [15:0] ram_1_dout;
  reg [15:0] ram_2_dout;
  wire [15:0] ram_1_din;
  wire [15:0] ram_2_din;
  wire ram_1_ack;
  wire ram_2_ack;
  wire ram_ack;

  wire inst_addr_out_of_range;
  wire data_addr_out_of_range;

  reg ram_as;
  reg [1:0] ram_la;

  reg [127:0] data;
  reg data_w0;
  reg data_w1;
  reg data_w2;
  reg data_w3;

  reg [4:0] state;
  reg [4:0] next_state;

  //
  // create ram instances
  //

  ram ram1(
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(ram_we),
    .addr(ram_addr[24:0]),
    .data_in(ram_1_dout[15:0]),
    .data_out(ram_1_din[15:0]),
    .ack(ram_1_ack)
  );

  ram ram2(
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(ram_we),
    .addr(ram_addr[24:0]),
    .data_in(ram_2_dout[15:0]),
    .data_out(ram_2_din[15:0]),
    .ack(ram_2_ack)
  );

  assign ram_ack = ram_1_ack & ram_2_ack;

  //
  // address range check
  //

  assign inst_addr_out_of_range = | inst_addr[24:23];
  assign data_addr_out_of_range = | data_addr[24:23];

  //
  // address output to ram
  //

  assign ram_addr[24:0] =
    ~ram_as ? { inst_addr[22:0], ram_la[1:0] } :
              { data_addr[22:0], ram_la[1:0] };

  //
  // data output to ram
  //

  always @(*) begin
    case (ram_la[1:0])
      2'b00:
        begin
          ram_1_dout[15:0] = data_din[127:112];
          ram_2_dout[15:0] = data_din[111: 96];
        end
      2'b01:
        begin
          ram_1_dout[15:0] = data_din[ 95: 80];
          ram_2_dout[15:0] = data_din[ 79: 64];
        end
      2'b10:
        begin
          ram_1_dout[15:0] = data_din[ 63: 48];
          ram_2_dout[15:0] = data_din[ 47: 32];
        end
      2'b11:
        begin
          ram_1_dout[15:0] = data_din[ 31: 16];
          ram_2_dout[15:0] = data_din[ 15:  0];
        end
    endcase
  end

  //
  // data output to cache
  //

  always @(posedge clk) begin
    if (data_w0) begin
      data[127:112] <= ram_1_din[15:0];
      data[111: 96] <= ram_2_din[15:0];
    end
    if (data_w1) begin
      data[ 95: 80] <= ram_1_din[15:0];
      data[ 79: 64] <= ram_2_din[15:0];
    end
    if (data_w2) begin
      data[ 63: 48] <= ram_1_din[15:0];
      data[ 47: 32] <= ram_2_din[15:0];
    end
    if (data_w3) begin
      data[ 31: 16] <= ram_1_din[15:0];
      data[ 15:  0] <= ram_2_din[15:0];
    end
  end

  assign inst_dout[127:0] = data[127:0];
  assign data_dout[127:0] = data[127:0];

  //
  // ramctrl state machine
  //

  always @(posedge clk) begin
    if (rst) begin
      state <= 0;
    end else begin
      state <= next_state;
    end
  end

  always @(*) begin
    case (state)
      5'd0:
        // idle, request arbitration
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_la = 2'bxx;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (data_stb) begin
            if (data_addr_out_of_range) begin
              // illegal data address
              next_state = 5'd17;
            end else begin
              // data address is ok
              if (data_we) begin
                // data write request
                next_state = 5'd12;
              end else begin
                // data read request
                next_state = 5'd7;
              end
            end
          end else begin
            if (inst_stb) begin
              if (inst_addr_out_of_range) begin
                // illegal inst address
                next_state = 5'd6;
              end else begin
                // inst address is ok
                // inst read request
                next_state = 5'd1;
              end
            end else begin
              // no request
              next_state = 5'd0;
            end
          end
        end
      5'd1:
        // inst read, phase 1
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b0;
          ram_la = 2'b00;
          data_w0 = ram_ack;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd2;
          end else begin
            next_state = 5'd1;
          end
        end
      5'd2:
        // inst read, phase 2
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b0;
          ram_la = 2'b01;
          data_w0 = 1'b0;
          data_w1 = ram_ack;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd3;
          end else begin
            next_state = 5'd2;
          end
        end
      5'd3:
        // inst read, phase 3
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b0;
          ram_la = 2'b10;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = ram_ack;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd4;
          end else begin
            next_state = 5'd3;
          end
        end
      5'd4:
        // inst read, phase 4
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b0;
          ram_la = 2'b11;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = ram_ack;
          if (ram_ack) begin
            next_state = 5'd5;
          end else begin
            next_state = 5'd4;
          end
        end
      5'd5:
        // inst read, phase 5
        begin
          inst_ack = 1'b1;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_la = 2'bxx;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          next_state = 5'd0;
        end
      5'd6:
        // illegal inst address
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b1;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_la = 2'bxx;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          next_state = 5'd0;
        end
      5'd7:
        // data read, phase 1
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b1;
          ram_la = 2'b00;
          data_w0 = ram_ack;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd8;
          end else begin
            next_state = 5'd7;
          end
        end
      5'd8:
        // data read, phase 2
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b1;
          ram_la = 2'b01;
          data_w0 = 1'b0;
          data_w1 = ram_ack;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd9;
          end else begin
            next_state = 5'd8;
          end
        end
      5'd9:
        // data read, phase 3
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b1;
          ram_la = 2'b10;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = ram_ack;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd10;
          end else begin
            next_state = 5'd9;
          end
        end
      5'd10:
        // data read, phase 4
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b1;
          ram_la = 2'b11;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = ram_ack;
          if (ram_ack) begin
            next_state = 5'd11;
          end else begin
            next_state = 5'd10;
          end
        end
      5'd11:
        // data read, phase 5
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b1;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_la = 2'bxx;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          next_state = 5'd0;
        end
      5'd12:
        // data write, phase 1
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b1;
          ram_as = 1'b1;
          ram_la = 2'b00;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd13;
          end else begin
            next_state = 5'd12;
          end
        end
      5'd13:
        // data write, phase 2
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b1;
          ram_as = 1'b1;
          ram_la = 2'b01;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd14;
          end else begin
            next_state = 5'd13;
          end
        end
      5'd14:
        // data write, phase 3
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b1;
          ram_as = 1'b1;
          ram_la = 2'b10;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd15;
          end else begin
            next_state = 5'd14;
          end
        end
      5'd15:
        // data write, phase 4
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b1;
          ram_as = 1'b1;
          ram_la = 2'b11;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          if (ram_ack) begin
            next_state = 5'd16;
          end else begin
            next_state = 5'd15;
          end
        end
      5'd16:
        // data write, phase 5
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b1;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_la = 2'bxx;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          next_state = 5'd0;
        end
      5'd17:
        // illegal data address
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b1;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_la = 2'bxx;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          next_state = 5'd0;
        end
      default:
        // not used
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_la = 2'bxx;
          data_w0 = 1'b0;
          data_w1 = 1'b0;
          data_w2 = 1'b0;
          data_w3 = 1'b0;
          next_state = 5'd0;
        end
    endcase
  end

endmodule
