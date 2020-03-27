//
// device.v -- simulation of a PS/2 device
//


`timescale 1ns/10ps
`default_nettype none


module device(clk, rst,
              rcv_data, rcv_error, rcv_ready,
              xmt_ready, xmt_data, xmt_strobe,
              ps2_clock, ps2_data);
    input clk;
    input rst;
    // receiver interface
    output [7:0] rcv_data;
    output rcv_error;
    output reg rcv_ready;
    // transmitter interface
    output reg xmt_ready;
    input [7:0] xmt_data;
    input xmt_strobe;
    // PS/2 interface
    inout ps2_clock;
    inout ps2_data;

  //
  // PS/2 open-collector interface
  //

  reg ps2_clk_out;
  reg ps2_dat_out;
  wire ps2_clk_in;
  wire ps2_dat_in;

  assign ps2_clock = ~ps2_clk_out ? 1'b0 : 1'bz;
  assign ps2_data = ~ps2_dat_out ? 1'b0 : 1'bz;

  assign ps2_clk_in = ps2_clock;
  assign ps2_dat_in = ps2_data;

  //
  // PS/2 clock and data synchronizer
  //

  reg ps2_clk_p;
  reg ps2_clk_s;
  reg ps2_dat_p;
  reg ps2_dat_s;

  always @(posedge clk) begin
    ps2_clk_p <= ps2_clk_in;
    ps2_clk_s <= ps2_clk_p;
    ps2_dat_p <= ps2_dat_in;
    ps2_dat_s <= ps2_dat_p;
  end

  //
  // PS/2 clock and data integrator
  //

  reg [3:0] ps2_clk_int;
  reg [3:0] ps2_dat_int;
  wire [3:0] ps2_clk_int_nxt;
  wire [3:0] ps2_dat_int_nxt;

  assign ps2_clk_int_nxt[3:0] =
    (ps2_clk_int[3:0] == 4'hF && ps2_clk_s == 1'b1) ? 4'hF :
    (ps2_clk_int[3:0] == 4'h0 && ps2_clk_s == 1'b0) ? 4'h0 :
    (ps2_clk_s == 1'b1) ? ps2_clk_int[3:0] + 4'h1 : ps2_clk_int[3:0] - 4'h1;
  assign ps2_dat_int_nxt[3:0] =
    (ps2_dat_int[3:0] == 4'hF && ps2_dat_s == 1'b1) ? 4'hF :
    (ps2_dat_int[3:0] == 4'h0 && ps2_dat_s == 1'b0) ? 4'h0 :
    (ps2_dat_s == 1'b1) ? ps2_dat_int[3:0] + 4'h1 : ps2_dat_int[3:0] - 4'h1;

  always @(posedge clk) begin
    if (rst) begin
      ps2_clk_int[3:0] <= 4'hF;
      ps2_dat_int[3:0] <= 4'hF;
    end else begin
      ps2_clk_int[3:0] <= ps2_clk_int_nxt[3:0];
      ps2_dat_int[3:0] <= ps2_dat_int_nxt[3:0];
    end
  end

  //
  // PS/2 clock and data level detector with hysteresis
  //

  reg ps2_clk_lvl;
  reg ps2_dat_lvl;

  always @(posedge clk) begin
    if (rst) begin
      ps2_clk_lvl <= 1'b1;
      ps2_dat_lvl <= 1'b1;
    end else begin
      if (ps2_clk_int[3:0] == 4'h4) begin
        ps2_clk_lvl <= 1'b0;
      end
      if (ps2_clk_int[3:0] == 4'hB) begin
        ps2_clk_lvl <= 1'b1;
      end
      if (ps2_dat_int[3:0] == 4'h4) begin
        ps2_dat_lvl <= 1'b0;
      end
      if (ps2_dat_int[3:0] == 4'hB) begin
        ps2_dat_lvl <= 1'b1;
      end
    end
  end

  //
  // shift register and bit counter
  //

  reg [10:0] sr;
  reg sr_load;
  reg sr_shift;
  reg [3:0] bc;
  reg bc_clear;

  always @(posedge clk) begin
    if (rst) begin
      sr[10:0] <= 11'h7FF;
    end else begin
      if (sr_load) begin
        sr[10:0] <= { 1'b1, ~^xmt_data[7:0], xmt_data[7:0], 1'b0 };
      end else begin
        if (sr_shift) begin
          sr[10:0] <= { ps2_dat_lvl, sr[10:1] };
        end
      end
    end
  end

  always @(posedge clk) begin
    if (bc_clear) begin
      bc[3:0] <= 4'h0;
    end else begin
      if (sr_shift) begin
        bc[3:0] <= bc[3:0] + 4'h1;
      end
    end
  end

  //
  // detect 100 us high on ps2_clk_lvl
  //

  reg [12:0] clk_quiet_cnt;
  wire clk_quiet;

  always @(posedge clk) begin
    if (rst | ~ps2_clk_lvl) begin
      clk_quiet_cnt[12:0] <= 13'd5000;
    end else begin
      if (~clk_quiet) begin
        clk_quiet_cnt[12:0] <= clk_quiet_cnt[12:0] - 13'd1;
      end
    end
  end

  assign clk_quiet = ~|clk_quiet_cnt[12:0];

  //
  // 20 us timer
  //

  reg [9:0] tmr20;
  reg tmr20_run;
  wire tmr20_exp;

  always @(posedge clk) begin
    if (~tmr20_run | tmr20_exp) begin
      tmr20[9:0] <= 10'd1000;
    end else begin
      tmr20[9:0] <= tmr20[9:0] - 10'd1;
    end
  end

  assign tmr20_exp = ~|tmr20[9:0];

  //
  // PS/2 protocol FSM
  //

  reg [3:0] state;
  reg [3:0] next_state;

  always @(posedge clk) begin
    if (rst) begin
      state[3:0] <= 4'h0;
    end else begin
      state[3:0] <= next_state[3:0];
    end
  end

  always @(*) begin
    case (state[3:0])
      4'h0:
        // idle
        // wait for falling clock edge
        // or quiet clock and xmt_strobe
        begin
          if (ps2_clk_lvl) begin
            if (~(clk_quiet & xmt_strobe)) begin
              // none of the conditions met: wait a little bit longer
              next_state[3:0] = 4'h0;
              sr_load = 1'b0;
              sr_shift = 1'b0;
              bc_clear = 1'b1;
              tmr20_run = 1'b0;
              ps2_clk_out = 1'b1;
              ps2_dat_out = 1'b1;
              rcv_ready = 1'b0;
              xmt_ready = clk_quiet;
            end else begin
              // quiet clock and xmt_strobe: transmit a byte
              // load shift register
              next_state[3:0] = 4'h1;
              sr_load = 1'b1;
              sr_shift = 1'b0;
              bc_clear = 1'b1;
              tmr20_run = 1'b0;
              ps2_clk_out = 1'b1;
              ps2_dat_out = 1'b1;
              rcv_ready = 1'b0;
              xmt_ready = 1'b0;
            end
          end else begin
            // falling clock edge: clock inhibited
            next_state[3:0] = 4'h5;
            sr_load = 1'b0;
            sr_shift = 1'b0;
            bc_clear = 1'b1;
            tmr20_run = 1'b0;
            ps2_clk_out = 1'b1;
            ps2_dat_out = 1'b1;
            rcv_ready = 1'b0;
            xmt_ready = 1'b0;
          end
        end
      4'h1:
        // wait 20 us
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'h1;
          end else begin
            next_state[3:0] = 4'h2;
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b1;
          ps2_dat_out = sr[0];
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'h2:
        // lower clock, wait 20 us
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'h2;
          end else begin
            next_state[3:0] = 4'h3;
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b0;
          ps2_dat_out = sr[0];
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'h3:
        // wait 20 us
        // transmission is finished if 10 bits have been shifted
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'h3;
          end else begin
            if (bc[3:0] == 4'hA) begin
              next_state[3:0] = 4'h0;
            end else begin
              next_state[3:0] = 4'h4;
            end
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b0;
          ps2_dat_out = sr[0];
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'h4:
        // raise clock, wait 20 us
        // then shift data
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'h4;
            sr_load = 1'b0;
            sr_shift = 1'b0;
          end else begin
            next_state[3:0] = 4'h1;
            sr_load = 1'b0;
            sr_shift = 1'b1;
          end
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b1;
          ps2_dat_out = sr[0];
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'h5:
        // clock inhibited
        // wait for rising clock edge
        // if data high: idle again
        // if data low: request to send from host
        begin
          if (~ps2_clk_lvl) begin
            next_state[3:0] = 4'h5;
          end else begin
            if (ps2_dat_lvl) begin
              next_state[3:0] = 4'h0;
            end else begin
              next_state[3:0] = 4'h6;
            end
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b1;
          tmr20_run = 1'b0;
          ps2_clk_out = 1'b1;
          ps2_dat_out = 1'b1;
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'h6:
        // clock high for 20 us
        // then sample data
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'h6;
            sr_load = 1'b0;
            sr_shift = 1'b0;
          end else begin
            next_state[3:0] = 4'h7;
            sr_load = 1'b0;
            sr_shift = 1'b1;
          end
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b1;
          ps2_dat_out = 1'b1;
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'h7:
        // bail out if 11 bits have been shifted
        // clock high for 20 us
        begin
          if (~tmr20_exp) begin
            if (bc[3:0] == 4'hB) begin
              next_state[3:0] = 4'hA;
            end else begin
              next_state[3:0] = 4'h7;
            end
          end else begin
            next_state[3:0] = 4'h8;
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b1;
          ps2_dat_out = 1'b1;
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'h8:
        // clock low for 20 us
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'h8;
          end else begin
            next_state[3:0] = 4'h9;
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b0;
          ps2_dat_out = 1'b1;
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'h9:
        // clock low for 20 us
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'h9;
          end else begin
            next_state[3:0] = 4'h6;
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b0;
          ps2_dat_out = 1'b1;
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'hA:
        // clock high and data low (ACK) for 20 us
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'hA;
          end else begin
            next_state[3:0] = 4'hB;
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b1;
          ps2_dat_out = 1'b0;
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'hB:
        // clock low and data low (ACK) for 20 us
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'hB;
          end else begin
            next_state[3:0] = 4'hC;
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b0;
          ps2_dat_out = 1'b0;
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
      4'hC:
        // clock low and data low (ACK) for 20 us
        // then reception is finished
        begin
          if (~tmr20_exp) begin
            next_state[3:0] = 4'hC;
            rcv_ready = 1'b0;
          end else begin
            next_state[3:0] = 4'h0;
            rcv_ready = 1'b1;
          end
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b1;
          ps2_clk_out = 1'b0;
          ps2_dat_out = 1'b0;
          xmt_ready = 1'b0;
        end
      default:
        // should never be reached
        begin
          next_state[3:0] = 4'h0;
          sr_load = 1'b0;
          sr_shift = 1'b0;
          bc_clear = 1'b0;
          tmr20_run = 1'b0;
          ps2_clk_out = 1'b1;
          ps2_dat_out = 1'b1;
          rcv_ready = 1'b0;
          xmt_ready = 1'b0;
        end
    endcase
  end

  //
  // received data output
  //

  wire framing_error;
  wire parity_error;

  assign rcv_data[7:0] = sr[8:1];
  assign framing_error = sr[0] | ~sr[10];
  assign parity_error = ~^sr[9:1];
  assign rcv_error = framing_error | parity_error;

endmodule
