//
// fpcif.v -- floating-point cif function
//


`timescale 1ns / 1ps
`default_nettype none


module fpcif(clk, run, stall,
             rnd, x, z, flags);
    input clk;
    input run;
    output stall;
    input [1:0] rnd;
    input [31:0] x;
    output reg [31:0] z;
    output [4:0] flags;

  wire sx;
  wire [31:0] absx;
  wire [5:0] lx;
  wire [31:0] m;
  wire [7:0] ez;
  wire [22:0] fz;
  wire round;
  wire sticky;
  wire odd;
  wire inxct;
  reg incr;
  wire [31:0] zpr;

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
  // conversion to float
  //

  assign sx = x[31];
  assign absx[31:0] = sx ? (~x[31:0] + 32'h1) : x[31:0];
  lzc32 lzc32_1(absx[31:0], lx[5:0]);
  assign m[31:0] = absx[31:0] << lx[4:0];
  assign ez[7:0] = 8'd158 - { 3'b000, lx[4:0] };
  assign fz[22:0] = m[30:8];
  assign round = m[7];
  assign sticky = | m[6:0];
  assign odd = fz[0];
  assign inxct = round | sticky;
  always @(*) begin
    case (rnd[1:0])
      2'b00: /* near */ incr = round & (sticky | odd);
      2'b01: /* zero */ incr = 1'b0;
      2'b10: /* down */ incr = sx & inxct;
      2'b11: /* up   */ incr = ~sx & inxct;
    endcase
  end
  assign zpr[31:0] = { sx, ez[7:0], fz[22:0] };

  //
  // compute output
  //

  always @(*) begin
    if (lx[5]) begin
      z[31:0] = { 1'b0, 8'h00, 23'h000000 };
      flag_v = 1'b0;
      flag_i = 1'b0;
      flag_o = 1'b0;
      flag_u = 1'b0;
      flag_x = 1'b0;
    end else begin
      z[31:0] = zpr[31:0] + { 30'h00000000, incr };
      flag_v = 1'b0;
      flag_i = 1'b0;
      flag_o = 1'b0;
      flag_u = 1'b0;
      flag_x = inxct;
    end
  end

  assign flags[4:0] = { flag_v, flag_i, flag_o, flag_u, flag_x };

endmodule


//**************************************************************


//
// 32-bit leading zero counter
//


module encode2(x, n);
    input [1:0] x;
    output [1:0] n;

  assign n[1] = ~x[1] & ~x[0];
  assign n[0] = ~x[1] & x[0];

endmodule


module combine2(nl, nr, nc);
    input [1:0] nl;
    input [1:0] nr;
    output [2:0] nc;

  assign nc[2:0] =
    ~nl[1] ? { 1'b0, nl[1:0] } :
    ~nr[1] ? { 2'b01, nr[0:0] } :
             3'b100;

endmodule


module combine3(nl, nr, nc);
    input [2:0] nl;
    input [2:0] nr;
    output [3:0] nc;

  assign nc[3:0] =
    ~nl[2] ? { 1'b0, nl[2:0] } :
    ~nr[2] ? { 2'b01, nr[1:0] } :
             4'b1000;

endmodule


module combine4(nl, nr, nc);
    input [3:0] nl;
    input [3:0] nr;
    output [4:0] nc;

  assign nc[4:0] =
    ~nl[3] ? { 1'b0, nl[3:0] } :
    ~nr[3] ? { 2'b01, nr[2:0] } :
             5'b10000;

endmodule


module combine5(nl, nr, nc);
    input [4:0] nl;
    input [4:0] nr;
    output [5:0] nc;

  assign nc[5:0] =
    ~nl[4] ? { 1'b0, nl[4:0] } :
    ~nr[4] ? { 2'b01, nr[3:0] } :
             6'b100000;

endmodule


module lzc32(x, n);
    input [31:0] x;
    output [5:0] n;

  wire [1:0] lz1_f, lz1_e, lz1_d, lz1_c, lz1_b, lz1_a, lz1_9, lz1_8;
  wire [1:0] lz1_7, lz1_6, lz1_5, lz1_4, lz1_3, lz1_2, lz1_1, lz1_0;
  wire [2:0] lz2_7, lz2_6, lz2_5, lz2_4, lz2_3, lz2_2, lz2_1, lz2_0;
  wire [3:0] lz3_3, lz3_2, lz3_1, lz3_0;
  wire [4:0] lz4_1, lz4_0;
  wire [5:0] lz5_0;

  encode2 encode2_f(x[31:30], lz1_f[1:0]);
  encode2 encode2_e(x[29:28], lz1_e[1:0]);
  encode2 encode2_d(x[27:26], lz1_d[1:0]);
  encode2 encode2_c(x[25:24], lz1_c[1:0]);
  encode2 encode2_b(x[23:22], lz1_b[1:0]);
  encode2 encode2_a(x[21:20], lz1_a[1:0]);
  encode2 encode2_9(x[19:18], lz1_9[1:0]);
  encode2 encode2_8(x[17:16], lz1_8[1:0]);
  encode2 encode2_7(x[15:14], lz1_7[1:0]);
  encode2 encode2_6(x[13:12], lz1_6[1:0]);
  encode2 encode2_5(x[11:10], lz1_5[1:0]);
  encode2 encode2_4(x[ 9: 8], lz1_4[1:0]);
  encode2 encode2_3(x[ 7: 6], lz1_3[1:0]);
  encode2 encode2_2(x[ 5: 4], lz1_2[1:0]);
  encode2 encode2_1(x[ 3: 2], lz1_1[1:0]);
  encode2 encode2_0(x[ 1: 0], lz1_0[1:0]);

  combine2 combine2_7(lz1_f[1:0], lz1_e[1:0], lz2_7[2:0]);
  combine2 combine2_6(lz1_d[1:0], lz1_c[1:0], lz2_6[2:0]);
  combine2 combine2_5(lz1_b[1:0], lz1_a[1:0], lz2_5[2:0]);
  combine2 combine2_4(lz1_9[1:0], lz1_8[1:0], lz2_4[2:0]);
  combine2 combine2_3(lz1_7[1:0], lz1_6[1:0], lz2_3[2:0]);
  combine2 combine2_2(lz1_5[1:0], lz1_4[1:0], lz2_2[2:0]);
  combine2 combine2_1(lz1_3[1:0], lz1_2[1:0], lz2_1[2:0]);
  combine2 combine2_0(lz1_1[1:0], lz1_0[1:0], lz2_0[2:0]);

  combine3 combine3_3(lz2_7[2:0], lz2_6[2:0], lz3_3[3:0]);
  combine3 combine3_2(lz2_5[2:0], lz2_4[2:0], lz3_2[3:0]);
  combine3 combine3_1(lz2_3[2:0], lz2_2[2:0], lz3_1[3:0]);
  combine3 combine3_0(lz2_1[2:0], lz2_0[2:0], lz3_0[3:0]);

  combine4 combine4_1(lz3_3[3:0], lz3_2[3:0], lz4_1[4:0]);
  combine4 combine4_0(lz3_1[3:0], lz3_0[3:0], lz4_0[4:0]);

  combine5 combine5_0(lz4_1[4:0], lz4_0[4:0], lz5_0[5:0]);

  assign n[5:0] = lz5_0[5:0];

endmodule
