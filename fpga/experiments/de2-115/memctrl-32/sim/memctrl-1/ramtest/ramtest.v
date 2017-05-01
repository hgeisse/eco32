//
// ramtest.v -- RAM test generator
//


`timescale 1ns/10ps
`default_nettype none


`define SIMULATE
`define VERBOSE

`define DATA_PERIOD	17
`define DATA_PHASE	7


//
// memory test generator
//
// Algorithm: Two independent address/data generators
// produce exactly the same sequence of address/data pairs,
// although at different times: data write and data read.
// Three out of four data cycles are writes, one is a read.
// The writing process is therefore always ahead of the
// reading process, with an increasing gap in between.
//

module ramtest(clk, rst,
               data_stb, data_we, data_addr,
               data_dout, data_din, data_ack,
               test_ended, test_error);
    input clk;
    input rst;
    output reg data_stb;
    output reg data_we;
    output [26:0] data_addr;
    output [31:0] data_dout;
    input [31:0] data_din;
    input data_ack;
    output test_ended;
    output test_error;

  reg [4:0] data_timer;
  reg [9:0] data_counter;

  wire dw_next;
  wire [24:0] dw_a;
  wire [31:0] dw_d;

  wire dr_next;
  wire [24:0] dr_a;
  wire [31:0] dr_d;

`ifdef SIMULATE
  reg error_1;
`endif
  reg error_2;

  always @(posedge clk) begin
    if (rst) begin
      data_timer <= 0;
      data_stb <= 0;
      data_we <= 0;
      data_counter <= 0;
`ifdef SIMULATE
      error_1 <= 0;
`endif
      error_2 <= 0;
    end else begin
      if (~test_ended | data_stb) begin
        if (~data_stb) begin
          if (data_timer == `DATA_PERIOD - 1) begin
            data_timer <= 0;
          end else begin
            data_timer <= data_timer + 5'd1;
          end
          if (data_timer == `DATA_PHASE) begin
            data_stb <= 1;
            data_we <= ~&data_counter[1:0];
          end
        end else begin
          if (data_ack) begin
            data_stb <= 0;
            data_we <= 0;
            data_counter <= data_counter + 10'd1;
`ifdef SIMULATE
`ifdef VERBOSE
            if (data_we == 1) begin
              $display("%t: data write @ 0x%h", $realtime, dw_a);
              $display("                   value = 0x%h", dw_d);
            end else begin
              $display("%t: data read  @ 0x%h", $realtime, dr_a);
              $display("                   value = 0x%h", data_din);
            end
`endif
            if (data_we == 0 &&
                ^data_din[31:0] === 1'bx) begin
              $display("Warning: Input data has don't cares at %t",
                       $realtime);
              error_1 <= 1;
            end
`endif
            if (data_we == 0 &&
                data_din[31:0] != dr_d[31:0]) begin
              error_2 <= 1;
            end
          end
        end
      end
    end
  end

  adgen adgen_dw(
    .clk(clk),
    .rst(rst),
    .next(dw_next),
    .addr(dw_a),
    .data(dw_d)
  );

  adgen adgen_dr(
    .clk(clk),
    .rst(rst),
    .next(dr_next),
    .addr(dr_a),
    .data(dr_d)
  );

  assign dw_next = data_ack & data_we;
  assign dr_next = data_ack & ~data_we;
  assign data_addr[26:0] = { 2'h0, data_we ? dw_a[24:0] : dr_a[24:0] };
  assign data_dout[31:0] = dw_d[31:0];

  assign test_ended = &data_counter[9:0];

`ifdef SIMULATE
  assign test_error = error_1 | error_2;
`else
  assign test_error = error_2;
`endif

endmodule


//
// address & data generator
//
// compute pseudo-random 32-bit address
// and 32-bit data on request
//

module adgen(clk, rst,
             next, addr, data);
    input clk;
    input rst;
    input next;
    output [24:0] addr;
    output [31:0] data;

  reg [31:0] a;
  reg [31:0] d;

  always @(posedge clk) begin
    if (rst) begin
      a[31: 0] <= 32'hC70337DB;
      d[31: 0] <= 32'h75377599;
    end else begin
      if (next) begin
        if (a[0] == 0) begin
          a[31:0] <= a[31:0] >> 1;
        end else begin
          a[31:0] <= (a[31:0] >> 1) ^ 32'hD0000001;
        end
        if (d[0] == 0) begin
          d[31:0] <= d[31:0] >> 1;
        end else begin
          d[31:0] <= (d[31:0] >> 1) ^ 32'hD0000001;
        end
      end
    end
  end

  assign addr[24:0] = a[24:0];
  assign data[31:0] = d[31:0];

endmodule
