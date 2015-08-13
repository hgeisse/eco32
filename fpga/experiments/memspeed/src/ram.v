//
// ram.v -- external RAM interface, using SDRAM
//          8M x 32 bit = 32 MB
//


`timescale 1ns/10ps
`default_nettype none


module ram(clk, clk_ok, rst,
           stb, we, addr,
           data_in, data_out, ack,
           sdram_cke, sdram_cs_n,
           sdram_ras_n, sdram_cas_n,
           sdram_we_n, sdram_ba, sdram_a,
           sdram_udqm, sdram_ldqm, sdram_dq);
    // internal interface signals
    input clk;
    input clk_ok;
    input rst;
    input stb;
    input we;
    input [24:2] addr;
    input [31:0] data_in;
    output reg [31:0] data_out;
    output reg ack;
    // SDRAM interface signals
    output sdram_cke;
    output sdram_cs_n;
    output sdram_ras_n;
    output sdram_cas_n;
    output sdram_we_n;
    output [1:0] sdram_ba;
    output [12:0] sdram_a;
    output sdram_udqm;
    output sdram_ldqm;
    inout [15:0] sdram_dq;

  reg [2:0] state;
  reg a0;
  reg cntl_read;
  reg cntl_write;
  wire cntl_done;
  wire [23:0] cntl_addr;
  reg [15:0] cntl_din;
  wire [15:0] cntl_dout;

  wire sd_out_en;
  wire [15:0] sd_out;

//--------------------------------------------------------------

  sdramCntl sdramCntl_1(
    // clock
    .clk(clk),
    .clk_ok(clk_ok),
    // host side
    .rd(cntl_read & ~cntl_done),
    .wr(cntl_write & ~cntl_done),
    .done(cntl_done),
    .hAddr(cntl_addr),
    .hDIn(cntl_din),
    .hDOut(cntl_dout),
    // SDRAM side
    .cke(sdram_cke),
    .ce_n(sdram_cs_n),
    .ras_n(sdram_ras_n),
    .cas_n(sdram_cas_n),
    .we_n(sdram_we_n),
    .ba(sdram_ba),
    .sAddr(sdram_a),
    .sDIn(sdram_dq),
    .sDOut(sd_out),
    .sDOutEn(sd_out_en),
    .dqmh(sdram_udqm),
    .dqml(sdram_ldqm)
  );

  assign sdram_dq = (sd_out_en == 1) ? sd_out : 16'hzzzz;

//--------------------------------------------------------------

  // the SDRAM is organized in 16-bit halfwords
  // address line 0 is controlled by the state machine
  assign cntl_addr[23:1] = addr[24:2];
  assign cntl_addr[0] = a0;

  // state machine for SDRAM access
  always @(posedge clk) begin
    if (rst) begin
      state <= 3'b000;
      ack <= 0;
    end else begin
      case (state)
        3'b000:
          // wait for access
          begin
            if (stb) begin
              // access
              if (we) begin
                // write
                state <= 3'b001;
              end else begin
                // read
                state <= 3'b011;
              end
            end
          end
        3'b001:
          // write word, upper 16 bits
          begin
            if (cntl_done) begin
              state <= 3'b010;
            end
          end
        3'b010:
          // write word, lower 16 bits
          begin
            if (cntl_done) begin
              state <= 3'b111;
              ack <= 1;
            end
          end
        3'b011:
          // read word, upper 16 bits
          begin
            if (cntl_done) begin
              state <= 3'b100;
              data_out[31:16] <= cntl_dout;
            end
          end
        3'b100:
          // read word, lower 16 bits
          begin
            if (cntl_done) begin
              state <= 3'b111;
              data_out[15:0] <= cntl_dout;
              ack <= 1;
            end
          end
        3'b111:
          // end of bus cycle
          begin
            state <= 3'b000;
            ack <= 0;
          end
        default:
          // all other states: reset
          begin
            state <= 3'b000;
            ack <= 0;
          end
      endcase
    end
  end

  // output of state machine
  always @(*) begin
    case (state)
      3'b000:
        // wait for access
        begin
          a0 = 1'bx;
          cntl_read = 0;
          cntl_write = 0;
          cntl_din = 16'hxxxx;
        end
      3'b001:
        // write word, upper 16 bits
        begin
          a0 = 1'b0;
          cntl_read = 0;
          cntl_write = 1;
          cntl_din = data_in[31:16];
        end
      3'b010:
        // write word, lower 16 bits
        begin
          a0 = 1'b1;
          cntl_read = 0;
          cntl_write = 1;
          cntl_din = data_in[15:0];
        end
      3'b011:
        // read word, upper 16 bits
        begin
          a0 = 1'b0;
          cntl_read = 1;
          cntl_write = 0;
          cntl_din = 16'hxxxx;
        end
      3'b100:
        // read word, lower 16 bits
        begin
          a0 = 1'b1;
          cntl_read = 1;
          cntl_write = 0;
          cntl_din = 16'hxxxx;
        end
      3'b111:
        // end of bus cycle
        begin
          a0 = 1'bx;
          cntl_read = 0;
          cntl_write = 0;
          cntl_din = 16'hxxxx;
        end
      default:
        // all other states: reset
        begin
          a0 = 1'bx;
          cntl_read = 0;
          cntl_write = 0;
          cntl_din = 16'hxxxx;
        end
    endcase
  end

endmodule
