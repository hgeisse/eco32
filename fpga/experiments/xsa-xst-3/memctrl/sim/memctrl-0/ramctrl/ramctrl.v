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
    input [25:0] inst_addr;
    output [63:0] inst_dout;
    output reg inst_ack;
    output reg inst_timeout;
    input data_stb;
    input data_we;
    input [25:0] data_addr;
    input [63:0] data_din;
    output [63:0] data_dout;
    output reg data_ack;
    output reg data_timeout;

  reg ram_stb;
  reg ram_we;
  wire [22:0] ram_addr;
  wire [31:0] ram_dout;
  wire [31:0] ram_din;
  wire ram_ack;

  wire inst_addr_out_of_range;
  wire data_addr_out_of_range;

  reg ram_as;
  reg ram_a0;

  reg ram_ds;

  reg [63:0] data;
  reg data_wh;
  reg data_wl;

  reg [3:0] state;
  reg [3:0] next_state;

  //
  // create ram instance
  //

  ram ram_1(
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(ram_we),
    .addr(ram_addr[22:0]),
    .data_in(ram_dout[31:0]),
    .data_out(ram_din[31:0]),
    .ack(ram_ack)
  );

  //
  // address range check
  //

  assign inst_addr_out_of_range = | inst_addr[25:22];
  assign data_addr_out_of_range = | data_addr[25:22];

  //
  // address output to ram
  //

  assign ram_addr[22:0] =
    ~ram_as ? { inst_addr[21:0], ram_a0 } :
              { data_addr[21:0], ram_a0 };

  //
  // data output to ram
  //

  assign ram_dout[31:0] =
    ~ram_ds ? data_din[63:32] : data_din[31:0];

  //
  // data output to cache
  //

  always @(posedge clk) begin
    if (data_wh) begin
      data[63:32] <= ram_din[31:0];
    end
    if (data_wl) begin
      data[31: 0] <= ram_din[31:0];
    end
  end

  assign inst_dout[63:0] = data[63:0];
  assign data_dout[63:0] = data[63:0];

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
      4'd0:
        // idle, request arbitration
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_a0 = 1'bx;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = 1'b0;
          if (data_stb) begin
            if (data_addr_out_of_range) begin
              // illegal data address
              next_state = 4'd11;
            end else begin
              // data address is ok
              if (data_we) begin
                // data write request
                next_state = 4'd7;
              end else begin
                // data read request
                next_state = 4'd4;
              end
            end
          end else begin
            if (inst_stb) begin
              if (inst_addr_out_of_range) begin
                // illegal inst address
                next_state = 4'd10;
              end else begin
                // inst address is ok
                // inst read request
                next_state = 4'd1;
              end
            end else begin
              // no request
              next_state = 4'd0;
            end
          end
        end
      4'd1:
        // inst read, phase 1
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b0;
          ram_a0 = 1'b0;
          ram_ds = 1'bx;
          data_wh = ram_ack;
          data_wl = 1'b0;
          if (ram_ack) begin
            next_state = 4'd2;
          end else begin
            next_state = 4'd1;
          end
        end
      4'd2:
        // inst read, phase 2
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b0;
          ram_a0 = 1'b1;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = ram_ack;
          if (ram_ack) begin
            next_state = 4'd3;
          end else begin
            next_state = 4'd2;
          end
        end
      4'd3:
        // inst read, phase 3
        begin
          inst_ack = 1'b1;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_a0 = 1'bx;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = 1'b0;
          next_state = 4'd0;
        end
      4'd4:
        // data read, phase 1
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b1;
          ram_a0 = 1'b0;
          ram_ds = 1'bx;
          data_wh = ram_ack;
          data_wl = 1'b0;
          if (ram_ack) begin
            next_state = 4'd5;
          end else begin
            next_state = 4'd4;
          end
        end
      4'd5:
        // data read, phase 2
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b0;
          ram_as = 1'b1;
          ram_a0 = 1'b1;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = ram_ack;
          if (ram_ack) begin
            next_state = 4'd6;
          end else begin
            next_state = 4'd5;
          end
        end
      4'd6:
        // data read, phase 3
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b1;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_a0 = 1'bx;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = 1'b0;
          next_state = 4'd0;
        end
      4'd7:
        // data write, phase 1
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b1;
          ram_as = 1'b1;
          ram_a0 = 1'b0;
          ram_ds = 1'b0;
          data_wh = 1'b0;
          data_wl = 1'b0;
          if (ram_ack) begin
            next_state = 4'd8;
          end else begin
            next_state = 4'd7;
          end
        end
      4'd8:
        // data write, phase 2
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b1;
          ram_we = 1'b1;
          ram_as = 1'b1;
          ram_a0 = 1'b1;
          ram_ds = 1'b1;
          data_wh = 1'b0;
          data_wl = 1'b0;
          if (ram_ack) begin
            next_state = 4'd9;
          end else begin
            next_state = 4'd8;
          end
        end
      4'd9:
        // data write, phase 3
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b1;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_a0 = 1'bx;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = 1'b0;
          next_state = 4'd0;
        end
      4'd10:
        // illegal inst address
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b1;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_a0 = 1'bx;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = 1'b0;
          next_state = 4'd0;
        end
      4'd11:
        // illegal data address
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b1;
          ram_stb = 1'b0;
          ram_we = 1'b0;
          ram_as = 1'bx;
          ram_a0 = 1'bx;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = 1'b0;
          next_state = 4'd0;
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
          ram_a0 = 1'bx;
          ram_ds = 1'bx;
          data_wh = 1'b0;
          data_wl = 1'b0;
          next_state = 4'd0;
        end
    endcase
  end

endmodule
