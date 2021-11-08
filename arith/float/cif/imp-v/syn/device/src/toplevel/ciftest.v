//
// ciftest.v -- floating-point device test top-level description
//


`timescale 1ns / 1ps
`default_nettype none


module ciftest(clk_in,
               rst_in_n,
               rs232_0_rxd,
               rs232_0_txd
              );

    // clock and reset
    input clk_in;
    input rst_in_n;
    // RS-232
    input rs232_0_rxd;
    output rs232_0_txd;

  // clk_rst
  wire clk_ok;				// clocks stable
  wire mclk;				// memory clock, 100 MHz
  wire pclk;				// pixel clock, 75 MHz
  wire clk;				// system clock, 50 MHz
  wire rst;				// system reset
  // receiver
  reg rcv_read;
  wire rcv_rdy;
  wire [7:0] rcv_out;
  // transmitter
  reg xmt_wrt;
  wire xmt_rdy;
  reg [7:0] xmt_in;
  // number crunching
  reg run;
  wire stall;
  wire [31:0] z_out;
  wire [4:0] flags_out;
  // data buffers
  reg [31:0] x;
  reg [3:0] wr_x;
  reg [31:0] z;
  reg [4:0] flags;
  reg wr_zflags;
  reg [2:0] xmt_sel;
  // controller
  reg [3:0] state;
  reg [3:0] next_state;

  //--------------------------------------
  // module instances
  //--------------------------------------

  clk_rst clk_rst_0(
    .clk_in(clk_in),
    .rst_in_n(rst_in_n),
    .clk_ok(clk_ok),
    .clk_100_ps(),
    .clk_100(mclk),
    .clk_75(pclk),
    .clk_50(clk),
    .rst(rst)
  );

  rcvbuf rcvbuf_0(
    .clk(clk),
    .rst(rst),
    .bit_len(16'd434),
    .read(rcv_read),
    .ready(rcv_rdy),
    .data_out(rcv_out[7:0]),
    .serial_in(rs232_0_rxd)
  );

  xmtbuf xmtbuf_0(
    .clk(clk),
    .rst(rst),
    .bit_len(16'd434),
    .write(xmt_wrt),
    .ready(xmt_rdy),
    .data_in(xmt_in[7:0]),
    .serial_out(rs232_0_txd)
  );

  fpcif fpcif_0(
    .clk(clk),
    .run(run),
    .stall(stall),
    .x(x[31:0]),
    .z(z_out[31:0]),
    .flags(flags_out[4:0])
  );

  //--------------------------------------
  // data buffers
  //--------------------------------------

  always @(posedge clk) begin
    if (wr_x[0]) begin
      x[ 7: 0] <= rcv_out[7:0];
    end
    if (wr_x[1]) begin
      x[15: 8] <= rcv_out[7:0];
    end
    if (wr_x[2]) begin
      x[23:16] <= rcv_out[7:0];
    end
    if (wr_x[3]) begin
      x[31:24] <= rcv_out[7:0];
    end
  end

  always @(posedge clk) begin
    if (wr_zflags) begin
      z[31:0] <= z_out[31:0];
      flags[4:0] <= flags_out[4:0];
    end
  end

  always @(*) begin
    case (xmt_sel[2:0])
      3'b000:
        begin
          xmt_in[7:0] = z[ 7: 0];
        end
      3'b001:
        begin
          xmt_in[7:0] = z[15: 8];
        end
      3'b010:
        begin
          xmt_in[7:0] = z[23:16];
        end
      3'b011:
        begin
          xmt_in[7:0] = z[31:24];
        end
      default:
        begin
          xmt_in[7:0] = { 3'b000, flags[4:0] };
        end
    endcase
  end

  //--------------------------------------
  // controller
  //--------------------------------------

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
        // wait for byte x0 read on serial line
        begin
          if (~rcv_rdy) begin
            next_state = 4'h0;
            rcv_read = 1'b0;
            wr_x[3:0] = 4'h0;
          end else begin
            next_state = 4'h1;
            rcv_read = 1'b1;
            wr_x[3:0] = 4'h1;
          end
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h0;
          xmt_wrt = 1'b0;
        end
      4'h1:
        // wait for byte x1 read on serial line
        begin
          if (~rcv_rdy) begin
            next_state = 4'h1;
            rcv_read = 1'b0;
            wr_x[3:0] = 4'h0;
          end else begin
            next_state = 4'h2;
            rcv_read = 1'b1;
            wr_x[3:0] = 4'h2;
          end
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h0;
          xmt_wrt = 1'b0;
        end
      4'h2:
        // wait for byte x2 read on serial line
        begin
          if (~rcv_rdy) begin
            next_state = 4'h2;
            rcv_read = 1'b0;
            wr_x[3:0] = 4'h0;
          end else begin
            next_state = 4'h3;
            rcv_read = 1'b1;
            wr_x[3:0] = 4'h4;
          end
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h0;
          xmt_wrt = 1'b0;
        end
      4'h3:
        // wait for byte x3 read on serial line
        begin
          if (~rcv_rdy) begin
            next_state = 4'h3;
            rcv_read = 1'b0;
            wr_x[3:0] = 4'h0;
          end else begin
            next_state = 4'h8;
            rcv_read = 1'b1;
            wr_x[3:0] = 4'h8;
          end
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h0;
          xmt_wrt = 1'b0;
        end
      4'h8:
        // wait for operation to finish
        begin
          if (stall) begin
            next_state = 4'h8;
          end else begin
            next_state = 4'h9;
          end
          rcv_read = 1'b0;
          wr_x[3:0] = 4'h0;
          run = 1'b1;
          if (stall) begin
            wr_zflags = 1'b0;
          end else begin
            wr_zflags = 1'b1;
          end
          xmt_sel = 3'h0;
          xmt_wrt = 1'b0;
        end
      4'h9:
        // send byte z0 back on serial line
        begin
          if (~xmt_rdy) begin
            next_state = 4'h9;
          end else begin
            next_state = 4'hA;
          end
          rcv_read = 1'b0;
          wr_x[3:0] = 4'h0;
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h0;
          if (~xmt_rdy) begin
            xmt_wrt = 1'b0;
          end else begin
            xmt_wrt = 1'b1;
          end
        end
      4'hA:
        // send byte z1 back on serial line
        begin
          if (~xmt_rdy) begin
            next_state = 4'hA;
          end else begin
            next_state = 4'hB;
          end
          rcv_read = 1'b0;
          wr_x[3:0] = 4'h0;
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h1;
          if (~xmt_rdy) begin
            xmt_wrt = 1'b0;
          end else begin
            xmt_wrt = 1'b1;
          end
        end
      4'hB:
        // send byte z2 back on serial line
        begin
          if (~xmt_rdy) begin
            next_state = 4'hB;
          end else begin
            next_state = 4'hC;
          end
          rcv_read = 1'b0;
          wr_x[3:0] = 4'h0;
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h2;
          if (~xmt_rdy) begin
            xmt_wrt = 1'b0;
          end else begin
            xmt_wrt = 1'b1;
          end
        end
      4'hC:
        // send byte z3 back on serial line
        begin
          if (~xmt_rdy) begin
            next_state = 4'hC;
          end else begin
            next_state = 4'hD;
          end
          rcv_read = 1'b0;
          wr_x[3:0] = 4'h0;
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h3;
          if (~xmt_rdy) begin
            xmt_wrt = 1'b0;
          end else begin
            xmt_wrt = 1'b1;
          end
        end
      4'hD:
        // send flag byte back on serial line
        begin
          if (~xmt_rdy) begin
            next_state = 4'hD;
          end else begin
            // back to start state
            next_state = 4'h0;
          end
          rcv_read = 1'b0;
          wr_x[3:0] = 4'h0;
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h4;
          if (~xmt_rdy) begin
            xmt_wrt = 1'b0;
          end else begin
            xmt_wrt = 1'b1;
          end
        end
      default:
        // should not be reached
        begin
          next_state = 4'h0;
          rcv_read = 1'b0;
          wr_x[3:0] = 4'h0;
          run = 1'b0;
          wr_zflags = 1'b0;
          xmt_sel = 3'h0;
          xmt_wrt = 1'b0;
        end
    endcase
  end

endmodule
