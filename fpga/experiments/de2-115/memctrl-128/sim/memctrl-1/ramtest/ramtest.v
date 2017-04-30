//
// ramtest.v -- RAM test generator
//


`timescale 1ns/10ps
`default_nettype none


`define SIMULATE
`define VERBOSE

`define INST_PERIOD	37
`define INST_PHASE	19
`define DATA_PERIOD	17
`define DATA_PHASE	7


//
// memory test generator
//
// Algorithm: Three independent address/data generators
// produce exactly the same sequence of address/data pairs,
// although at different times: data write, data read, and
// instruction read. Three out of four data cycles are writes,
// one is a read. INST_PERIOD must be set high enough so that
// instruction reads are also less frequent than data writes,
// and thus no "don't cares" show up in the instructions read.
// The exact value depends on the actual number of read/write
// cycles consumed by the underlying RAM.
//

module ramtest(clk, rst,
               inst_stb, inst_addr,
               inst_din, inst_ack,
               data_stb, data_we, data_addr,
               data_dout, data_din, data_ack,
               test_ended, test_error);
    input clk;
    input rst;
    output reg inst_stb;
    output [24:0] inst_addr;
    input [127:0] inst_din;
    input inst_ack;
    output reg data_stb;
    output reg data_we;
    output [24:0] data_addr;
    output [127:0] data_dout;
    input [127:0] data_din;
    input data_ack;
    output test_ended;
    output test_error;

  reg [5:0] inst_timer;
  reg [4:0] data_timer;
  reg [9:0] data_counter;

  wire ir_next;
  wire [22:0] ir_a;
  wire [127:0] ir_d;

  wire dw_next;
  wire [22:0] dw_a;
  wire [127:0] dw_d;

  wire dr_next;
  wire [22:0] dr_a;
  wire [127:0] dr_d;

`ifdef SIMULATE
  reg error_1;
  reg error_3;
`endif
  reg error_2;
  reg error_4;

  always @(posedge clk) begin
    if (rst) begin
      inst_timer <= 0;
      inst_stb <= 0;
`ifdef SIMULATE
      error_1 <= 0;
`endif
      error_2 <= 0;
    end else begin
      if (~test_ended) begin
        if (~inst_stb) begin
          if (inst_timer == `INST_PERIOD - 1) begin
            inst_timer <= 0;
          end else begin
            inst_timer <= inst_timer + 6'd1;
          end
          if (inst_timer == `INST_PHASE) begin
            inst_stb <= 1;
          end
        end else begin
          if (inst_ack) begin
            inst_stb <= 0;
`ifdef SIMULATE
`ifdef VERBOSE
            $display("%t: inst read  @ 0x%h", $realtime, ir_a);
            $display("                   value = 0x%h", inst_din);
`endif
            if (^inst_din[127:0] === 1'bx) begin
              $display("Warning: Input data has don't cares at %t",
                       $realtime);
              error_1 <= 1;
            end
`endif
            if (inst_din[127:0] != ir_d[127:0]) begin
              error_2 <= 1;
            end
          end
        end
      end
    end
  end

  adgen adgen_ir(
    .clk(clk),
    .rst(rst),
    .next(ir_next),
    .addr(ir_a),
    .data(ir_d)
  );

  assign ir_next = inst_ack;
  assign inst_addr[24:0] = { 2'h0, ir_a[22:0] };

  always @(posedge clk) begin
    if (rst) begin
      data_timer <= 0;
      data_stb <= 0;
      data_we <= 0;
      data_counter <= 0;
`ifdef SIMULATE
      error_3 <= 0;
`endif
      error_4 <= 0;
    end else begin
      if (~test_ended) begin
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
                ^data_din[127:0] === 1'bx) begin
              $display("Warning: Input data has don't cares at %t",
                       $realtime);
              error_3 <= 1;
            end
`endif
            if (data_we == 0 &&
                data_din[127:0] != dr_d[127:0]) begin
              error_4 <= 1;
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
  assign data_addr[24:0] = { 2'h0, data_we ? dw_a[22:0] : dr_a[22:0] };
  assign data_dout[127:0] = dw_d[127:0];

  assign test_ended = &data_counter[9:0];

`ifdef SIMULATE
  assign test_error = error_1 | error_2 | error_3 | error_4;
`else
  assign test_error = error_2 | error_4;
`endif

endmodule


//
// address & data generator
//
// compute pseudo-random 32-bit address
// and 64-bit data on request
//
// addresses are restricted to 23 bits
// data is non-randomly expanded to 128 bits
//

module adgen(clk, rst,
             next, addr, data);
    input clk;
    input rst;
    input next;
    output [22:0] addr;
    output [127:0] data;

  reg [31:0] a;
  reg [63:0] d;

  always @(posedge clk) begin
    if (rst) begin
      a[31: 0] <= 32'hC70337DB;
      d[63:32] <= 32'h7F4D514F;
      d[31: 0] <= 32'h75377599;
    end else begin
      if (next) begin
        if (a[0] == 0) begin
          a[31:0] <= a[31:0] >> 1;
        end else begin
          a[31:0] <= (a[31:0] >> 1) ^ 32'hD0000001;
        end
        if (d[32] == 0) begin
          d[63:32] <= d[63:32] >> 1;
        end else begin
          d[63:32] <= (d[63:32] >> 1) ^ 32'hD0000001;
        end
        if (d[0] == 0) begin
          d[31:0] <= d[31:0] >> 1;
        end else begin
          d[31:0] <= (d[31:0] >> 1) ^ 32'hD0000001;
        end
      end
    end
  end

  assign addr[22:0] = a[22:0];
  assign data[127:0] = { d[63:0], ~d[63:0] };

endmodule
