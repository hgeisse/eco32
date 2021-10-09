//
// fpcmp.v -- floating-point comparator
//


`timescale 1ns / 1ps
`default_nettype none


`define PRED_EQ		2'b00
`define PRED_NE		2'b01
`define PRED_LE		2'b10
`define PRED_LT		2'b11


module fpcmp(clk, run, stall,
             pred, x, y, z, flags);
    input clk;
    input run;
    output stall;
    input [1:0] pred;
    input [31:0] x;
    input [31:0] y;
    output z;
    output [4:0] flags;

  wire sx;
  wire [7:0] ex;
  wire [22:0] fx;
  wire sy;
  wire [7:0] ey;
  wire [22:0] fy;

  wire is_zero_x;
  wire is_nan_x;
  wire is_quiet_x;
  wire is_zero_y;
  wire is_nan_y;
  wire is_quiet_y;

  wire [30:0] absx;
  wire [30:0] absy;
  wire lt;
  wire eq;

  wire cc_lt;
  wire cc_eq;
  wire cc_gt;

  reg cond_lt;
  reg cond_eq;
  reg cond_gt;
  reg cond_un;

  wire [3:0] cond_code;
  reg [3:0] cond_mask;

  reg flag_v;
  reg flag_i;
  reg flag_o;
  reg flag_u;
  reg flag_x;

  //
  // sequential control
  //

  assign stall = 1'b0;

  //
  // operand representation
  //

  assign sx = x[31];
  assign ex[7:0] = x[30:23];
  assign fx[22:0] = x[22:0];

  assign sy = y[31];
  assign ey[7:0] = y[30:23];
  assign fy[22:0] = y[22:0];

  //
  // operand classification
  //

  assign is_zero_x = (ex[7:0] == 8'h00) & (fx[22:0] == 23'h0);
  assign is_nan_x = (ex[7:0] == 8'hFF) & (fx[22:0] != 23'h0);
  assign is_quiet_x = fx[22];

  assign is_zero_y = (ey[7:0] == 8'h00) & (fy[22:0] == 23'h0);
  assign is_nan_y = (ey[7:0] == 8'hFF) & (fy[22:0] != 23'h0);
  assign is_quiet_y = fy[22];

  //
  // compute condition codes
  //

  assign absx[30:0] = x[30:0];
  assign absy[30:0] = y[30:0];

  assign lt = (absx[30:0] < absy[30:0]);
  assign eq = (absx[30:0] == absy[30:0]);

  assign cc_lt = (sx & sy & ~lt & ~eq) |
                 (sx & !sy) |
                 (~sx & ~sy & lt);
  assign cc_eq = (sx & sy & ~lt & eq) |
                 (~sx & ~sy & ~lt & eq);
  assign cc_gt = (sx & sy & lt) |
                 (~sx & sy) |
                 (~sx & ~sy & ~lt & ~eq);

  //
  // compute output
  //

  always @(*) begin
    if (is_nan_x & is_nan_y) begin
      cond_lt = 1'b0;
      cond_eq = 1'b0;
      cond_gt = 1'b0;
      cond_un = 1'b1;
      flag_v = ~is_quiet_x | ~is_quiet_y |
               ~((pred == `PRED_EQ) | (pred == `PRED_NE));
      flag_i = 1'b0;
      flag_o = 1'b0;
      flag_u = 1'b0;
      flag_x = 1'b0;
    end else
    if (is_nan_x & ~is_nan_y) begin
      cond_lt = 1'b0;
      cond_eq = 1'b0;
      cond_gt = 1'b0;
      cond_un = 1'b1;
      flag_v = ~is_quiet_x |
               ~((pred == `PRED_EQ) | (pred == `PRED_NE));
      flag_i = 1'b0;
      flag_o = 1'b0;
      flag_u = 1'b0;
      flag_x = 1'b0;
    end else
    if (~is_nan_x & is_nan_y) begin
      cond_lt = 1'b0;
      cond_eq = 1'b0;
      cond_gt = 1'b0;
      cond_un = 1'b1;
      flag_v = ~is_quiet_y |
               ~((pred == `PRED_EQ) | (pred == `PRED_NE));
      flag_i = 1'b0;
      flag_o = 1'b0;
      flag_u = 1'b0;
      flag_x = 1'b0;
    end else
    if (is_zero_x & is_zero_y) begin
      cond_lt = 1'b0;
      cond_eq = 1'b1;
      cond_gt = 1'b0;
      cond_un = 1'b0;
      flag_v = 1'b0;
      flag_i = 1'b0;
      flag_o = 1'b0;
      flag_u = 1'b0;
      flag_x = 1'b0;
    end else begin
      cond_lt = cc_lt;
      cond_eq = cc_eq;
      cond_gt = cc_gt;
      cond_un = 1'b0;
      flag_v = 1'b0;
      flag_i = 1'b0;
      flag_o = 1'b0;
      flag_u = 1'b0;
      flag_x = 1'b0;
    end
  end

  assign cond_code[3:0] = { cond_lt, cond_eq, cond_gt, cond_un };
  assign flags[4:0] = { flag_v, flag_i, flag_o, flag_u, flag_x };

  //
  // (predicate, condition code) -> boolean result
  //

  always @(*) begin
    case (pred[1:0])
      `PRED_EQ: cond_mask[3:0] = 4'b0100;
      `PRED_NE: cond_mask[3:0] = 4'b1011;
      `PRED_LE: cond_mask[3:0] = 4'b1100;
      `PRED_LT: cond_mask[3:0] = 4'b1000;
    endcase
  end

  assign z = |(cond_code[3:0] & cond_mask[3:0]);

endmodule
