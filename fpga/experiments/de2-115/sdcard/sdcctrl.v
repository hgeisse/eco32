//
// sdcctrl.v -- SD card controller
//


`include "sdcard.v"


`timescale 1ns/10ps
`default_nettype none


//
// These constants define the two SCLK frequencies:
// f = 25 MHz / (n + 1)
//
`define FAST_WAIT	6'd0
`define SLOW_WAIT	6'd63


module sdcctrl(clk, rst,
               stb, we, addr,
               din, dout);
    input clk;
    input rst;
    input stb;
    input we;
    input addr;
    input [7:0] din;
    output [7:0] dout;

  reg fast;
  reg rdy;

  reg ss_n;
  reg sclk;
  wire mosi;
  wire miso;

  reg [8:0] sreg;

  wire load;
  wire shift;
  wire latch;
  wire ready;
  reg [4:0] state;
  reg [5:0] wtcnt;
  wire [5:0] wtclk;

  sdcard sdcard_1(
    .cs_n(ss_n),
    .sclk(sclk),
    .di(mosi),
    .do(miso)
  );

  assign load = stb & we & addr;
  assign shift =
    (state ==  2 || state ==  4 ||
     state ==  6 || state ==  8 ||
     state == 10 || state == 12 ||
     state == 14 || state == 16) &&
    (wtcnt == 0);
  assign latch =
    (state ==  1 || state ==  3 ||
     state ==  5 || state ==  7 ||
     state ==  9 || state == 11 ||
     state == 13 || state == 15) &&
    (wtcnt == 0);
  assign ready = (state == 17) && (wtcnt == 0);

  always @(posedge clk) begin
    if (rst) begin
      sreg[8:0] <= 9'h1FF;
    end else begin
      if (load) begin
        sreg[8:1] <= din[7:0];
      end else begin
        if (shift) begin
          sreg[8:1] <= sreg[7:0];
        end
        if (latch) begin
          sreg[0] <= miso;
        end
      end
    end
  end

  assign mosi = sreg[8];

  assign wtclk[5:0] = fast ? `FAST_WAIT : `SLOW_WAIT;

  always @(posedge clk) begin
    if (rst) begin
      sclk  <= 0;
      wtcnt[5:0] <= 6'd0;
      state[4:0] <= 5'd0;
    end else begin
      if (| wtcnt[5:0]) begin
        wtcnt <= wtcnt - 1;
      end else begin
        case (state[4:0])
          5'd0:
            begin
              if (~load) begin
                state <= 5'd0;
              end else begin
                state <= 5'd1;
                wtcnt <= wtclk;
              end
            end
          5'd1:
            begin
              sclk  <= 1;
              state <= 5'd2;
              wtcnt <= wtclk;
            end
          5'd2:
            begin
              sclk  <= 0;
              state <= 5'd3;
              wtcnt <= wtclk;
            end
          5'd3:
            begin
              sclk  <= 1;
              state <= 5'd4;
              wtcnt <= wtclk;
            end
          5'd4:
            begin
              sclk  <= 0;
              state <= 5'd5;
              wtcnt <= wtclk;
            end
          5'd5:
            begin
              sclk  <= 1;
              state <= 5'd6;
              wtcnt <= wtclk;
            end
          5'd6:
            begin
              sclk  <= 0;
              state <= 5'd7;
              wtcnt <= wtclk;
            end
          5'd7:
            begin
              sclk  <= 1;
              state <= 5'd8;
              wtcnt <= wtclk;
            end
          5'd8:
            begin
              sclk  <= 0;
              state <= 5'd9;
              wtcnt <= wtclk;
            end
          5'd9:
            begin
              sclk  <= 1;
              state <= 5'd10;
              wtcnt <= wtclk;
            end
          5'd10:
            begin
              sclk  <= 0;
              state <= 5'd11;
              wtcnt <= wtclk;
            end
          5'd11:
            begin
              sclk  <= 1;
              state <= 5'd12;
              wtcnt <= wtclk;
            end
          5'd12:
            begin
              sclk  <= 0;
              state <= 5'd13;
              wtcnt <= wtclk;
            end
          5'd13:
            begin
              sclk  <= 1;
              state <= 5'd14;
              wtcnt <= wtclk;
            end
          5'd14:
            begin
              sclk  <= 0;
              state <= 5'd15;
              wtcnt <= wtclk;
            end
          5'd15:
            begin
              sclk  <= 1;
              state <= 5'd16;
              wtcnt <= wtclk;
            end
          5'd16:
            begin
              sclk  <= 0;
              state <= 5'd17;
              wtcnt <= wtclk;
            end
          5'd17:
            begin
              sclk  <= 0;
              state <= 5'd0;
              wtcnt <= 0;
            end
          default:
            begin
              sclk  <= 0;
              state <= 5'd0;
              wtcnt <= wtclk;
            end
        endcase
      end
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      fast <= 0;
      ss_n <= 1;
    end else begin
      if (stb & we & ~addr) begin
        fast <= din[1];
        ss_n <= ~din[0];
      end
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      rdy <= 0;
    end else begin
      if (ready | (stb & ~we & addr)) begin
        rdy <= ready;
      end
    end
  end

  assign dout[7:0] = addr ? sreg[8:1] : { 7'h0, rdy };

endmodule
