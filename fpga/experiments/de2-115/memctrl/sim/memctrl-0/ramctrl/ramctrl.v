//
// ramctrl.v -- RAM controller
//


`include "ramctrl/ram.v"


`timescale 1ns/10ps
`default_nettype none


module ramctrl(clk, rst,
               data_stb, data_we,
               data_addr, data_din,
               data_dout, data_ack,
               data_timeout);
    input clk;
    input rst;
    input data_stb;
    input data_we;
    input [26:0] data_addr;
    input [31:0] data_din;
    output [31:0] data_dout;
    output reg data_ack;
    output reg data_timeout;

  reg ram_stb;
  reg ram_we;
  wire [24:0] ram_addr;
  wire [15:0] ram_1_dout;
  wire [15:0] ram_2_dout;
  wire [15:0] ram_1_din;
  wire [15:0] ram_2_din;
  wire ram_1_ack;
  wire ram_2_ack;

  wire data_addr_out_of_range;

  reg [2:0] state;
  reg [2:0] next_state;

  //
  // create ram instances
  //

  ram ram_1(
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(ram_we),
    .addr(ram_addr[24:0]),
    .data_in(ram_1_dout[15:0]),
    .data_out(ram_1_din[15:0]),
    .ack(ram_1_ack)
  );

  ram ram_2(
    .clk(clk),
    .rst(rst),
    .stb(ram_stb),
    .we(ram_we),
    .addr(ram_addr[24:0]),
    .data_in(ram_2_dout[15:0]),
    .data_out(ram_2_din[15:0]),
    .ack(ram_2_ack)
  );

  //
  // address range check
  //

  assign data_addr_out_of_range = | data_addr[26:25];

  //
  // connections to ram
  //

  assign ram_addr[24:0] = data_addr[24:0];
  assign ram_1_dout[15:0] = data_din[31:16];
  assign ram_2_dout[15:0] = data_din[15:0];
  assign data_dout[31:16] = ram_1_din[15:0];
  assign data_dout[15:0] = ram_2_din[15:0];

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
      3'd0:
        // idle
        begin
          data_ack     = 1'b0;
          data_timeout = 1'b0;
          ram_stb      = 1'b0;
          ram_we       = 1'b0;
          if (data_stb) begin
            if (data_addr_out_of_range) begin
              // illegal data address
              next_state   = 3'd5;
            end else begin
              // data address is ok
              if (data_we) begin
                // data write request
                next_state   = 3'd3;
              end else begin
                // data read request
                next_state   = 3'd1;
              end
            end
          end else begin
            // no request
            next_state   = 3'd0;
          end
        end
      3'd1:
        // data read, phase 1
        begin
          data_ack     = 1'b0;
          data_timeout = 1'b0;
          ram_stb      = 1'b1;
          ram_we       = 1'b0;
          if (ram_1_ack & ram_2_ack) begin
            next_state   = 3'd2;
          end else begin
            next_state   = 3'd1;
          end
        end
      3'd2:
        // data read, phase 2
        begin
          data_ack     = 1'b1;
          data_timeout = 1'b0;
          ram_stb      = 1'b0;
          ram_we       = 1'b0;
          next_state   = 3'd0;
        end
      3'd3:
        // data write, phase 1
        begin
          data_ack     = 1'b0;
          data_timeout = 1'b0;
          ram_stb      = 1'b1;
          ram_we       = 1'b1;
          if (ram_1_ack & ram_2_ack) begin
            next_state   = 3'd4;
          end else begin
            next_state   = 3'd3;
          end
        end
      3'd4:
        // data write, phase 2
        begin
          data_ack     = 1'b1;
          data_timeout = 1'b0;
          ram_stb      = 1'b0;
          ram_we       = 1'b0;
          next_state   = 3'd0;
        end
      3'd5:
        // illegal data address
        begin
          data_ack     = 1'b0;
          data_timeout = 1'b1;
          ram_stb      = 1'b0;
          ram_we       = 1'b0;
          next_state   = 3'd0;
        end
      default:
        // not used
        begin
          data_ack     = 1'b0;
          data_timeout = 1'b0;
          ram_stb      = 1'b0;
          ram_we       = 1'b0;
          next_state   = 3'd0;
        end
    endcase
  end

endmodule
