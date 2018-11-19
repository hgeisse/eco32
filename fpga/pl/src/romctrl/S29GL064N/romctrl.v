//
// romctrl.v -- ROM controller
//


`timescale 1ns/10ps
`default_nettype none


`define RD_CYCLES	4'd10


module romctrl(clk, rst,
               inst_stb, inst_addr,
               inst_dout, inst_ack,
               inst_timeout,
               data_stb, data_addr,
               data_dout, data_ack,
               data_timeout,
               fl_ce_n, fl_oe_n,
               fl_we_n, fl_wp_n,
               fl_rst_n, fl_a, fl_d);
    // internal interface signals
    input clk;
    input rst;
    input inst_stb;
    input [23:0] inst_addr;
    output [127:0] inst_dout;
    output reg inst_ack;
    output reg inst_timeout;
    input data_stb;
    input [23:0] data_addr;
    output [127:0] data_dout;
    output reg data_ack;
    output reg data_timeout;
    // external interface signals
    output fl_ce_n;
    output fl_oe_n;
    output fl_we_n;
    output fl_wp_n;
    output fl_rst_n;
    output [22:0] fl_a;
    input [7:0] fl_d;

  wire inst_addr_out_of_range;
  wire data_addr_out_of_range;

  reg rom_as;
  reg rom_la_clr;
  reg rom_la_inc;
  reg [3:0] rom_la;

  reg [127:0] data;
  reg data_wr;

  reg [3:0] timer;
  reg timer_stb;
  wire timer_ack;

  reg [2:0] state;
  reg [2:0] next_state;

  //
  // the following control signals are all
  // either constantly asserted or deasserted
  //

  assign fl_ce_n = 1'b0;
  assign fl_oe_n = 1'b0;
  assign fl_we_n = 1'b1;
  assign fl_wp_n = 1'b1;
  assign fl_rst_n = 1'b1;

  //
  // address range check
  //

  assign inst_addr_out_of_range = | inst_addr[23:19];
  assign data_addr_out_of_range = | data_addr[23:19];

  //
  // address output to rom
  //

  always @(posedge clk) begin
    if (rom_la_clr) begin
      rom_la[3:0] <= 4'h0;
    end else begin
      if (rom_la_inc) begin
        rom_la[3:0] <= rom_la[3:0] + 4'h1;
      end
    end
  end

  assign fl_a[22:0] =
    ~rom_as ? { inst_addr[18:0], rom_la[3:0] } :
              { data_addr[18:0], rom_la[3:0] };

  //
  // data output to cache
  //

  always @(posedge clk) begin
    if (data_wr) begin
      if (rom_la[3:0] == 4'h0) begin
        data[127:120] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h1) begin
        data[119:112] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h2) begin
        data[111:104] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h3) begin
        data[103: 96] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h4) begin
        data[ 95: 88] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h5) begin
        data[ 87: 80] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h6) begin
        data[ 79: 72] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h7) begin
        data[ 71: 64] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h8) begin
        data[ 63: 56] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'h9) begin
        data[ 55: 48] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'hA) begin
        data[ 47: 40] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'hB) begin
        data[ 39: 32] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'hC) begin
        data[ 31: 24] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'hD) begin
        data[ 23: 16] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'hE) begin
        data[ 15:  8] <= fl_d[7:0];
      end
      if (rom_la[3:0] == 4'hF) begin
        data[  7:  0] <= fl_d[7:0];
      end
    end
  end

  assign inst_dout[127:0] = data[127:0];
  assign data_dout[127:0] = data[127:0];

  //
  // romctrl state machine
  //

  always @(posedge clk) begin
    if (rst) begin
      timer[3:0] <= 4'd0;
    end else begin
      if (timer[3:0] == 4'd0) begin
        if (timer_stb) begin
          timer[3:0] <= `RD_CYCLES - 4'd1;
        end
      end else begin
        timer[3:0] <= timer[3:0] - 4'd1;
      end
    end
  end

  assign timer_ack = (timer[3:0] == 4'd1);

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
        // idle, request arbitration
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          rom_as = 1'bx;
          rom_la_clr = 1'b1;
          rom_la_inc = 1'b0;
          data_wr = 1'b0;
          timer_stb = 1'b0;
          if (data_stb) begin
            if (data_addr_out_of_range) begin
              // illegal data address
              next_state = 3'd6;
            end else begin
              // data address is ok
              // data read request
              next_state = 3'd4;
            end
          end else begin
            if (inst_stb) begin
              if (inst_addr_out_of_range) begin
                // illegal inst address
                next_state = 3'd3;
              end else begin
                // inst address is ok
                // inst read request
                next_state = 3'd1;
              end
            end else begin
              // no request
              next_state = 3'd0;
            end
          end
        end
      3'd1:
        // inst read, collect data
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          rom_as = 1'b0;
          rom_la_clr = 1'b0;
          rom_la_inc = timer_ack;
          data_wr = timer_ack;
          timer_stb = 1'b1;
          if ((rom_la[3:0] == 4'hF) & timer_ack) begin
            next_state = 3'd2;
          end else begin
            next_state = 3'd1;
          end
        end
      3'd2:
        // inst read, acknowledge
        begin
          inst_ack = 1'b1;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          rom_as = 1'bx;
          rom_la_clr = 1'b0;
          rom_la_inc = 1'b0;
          data_wr = 1'b0;
          timer_stb = 1'b0;
          next_state = 3'd0;
        end
      3'd3:
        // illegal inst address
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b1;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          rom_as = 1'bx;
          rom_la_clr = 1'b0;
          rom_la_inc = 1'b0;
          data_wr = 1'b0;
          timer_stb = 1'b0;
          next_state = 3'd0;
        end
      3'd4:
        // data read, collect data
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          rom_as = 1'b1;
          rom_la_clr = 1'b0;
          rom_la_inc = timer_ack;
          data_wr = timer_ack;
          timer_stb = 1'b1;
          if ((rom_la[3:0] == 4'hF) & timer_ack) begin
            next_state = 3'd5;
          end else begin
            next_state = 3'd4;
          end
        end
      3'd5:
        // data read, acknowledge
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b1;
          data_timeout = 1'b0;
          rom_as = 1'bx;
          rom_la_clr = 1'b0;
          rom_la_inc = 1'b0;
          data_wr = 1'b0;
          timer_stb = 1'b0;
          next_state = 3'd0;
        end
      3'd6:
        // illegal data address
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b1;
          rom_as = 1'bx;
          rom_la_clr = 1'b0;
          rom_la_inc = 1'b0;
          data_wr = 1'b0;
          timer_stb = 1'b0;
          next_state = 3'd0;
        end
      default:
        // not used
        begin
          inst_ack = 1'b0;
          inst_timeout = 1'b0;
          data_ack = 1'b0;
          data_timeout = 1'b0;
          rom_as = 1'bx;
          rom_la_clr = 1'b0;
          rom_la_inc = 1'b0;
          data_wr = 1'b0;
          timer_stb = 1'b0;
          next_state = 3'd0;
        end
    endcase
  end

endmodule
