//
// if3.v -- instruction fetch, stage 3 (icache)
//          Note: This is only a test circuit for now. It computes
//                the CRC of the first 16 cache lines (256 bytes)
//                read from the ROM. If the ROM contains 0x00..0xFF
//                in the first 256 bytes, the CRC should be 0x7E55.
//


`timescale 1ns/10ps
`default_nettype none


module if3(clk, rst,
           test_crc_ok,
           ram_inst_stb, ram_inst_addr,
           ram_inst_dout, ram_inst_ack,
           ram_inst_timeout,
           rom_inst_stb, rom_inst_addr,
           rom_inst_dout, rom_inst_ack,
           rom_inst_timeout);
    input clk;
    input rst;
    //----------------
    output test_crc_ok;
    //----------------
    output ram_inst_stb;
    output [24:0] ram_inst_addr;
    input [127:0] ram_inst_dout;
    input ram_inst_ack;
    input ram_inst_timeout;
    //----------------
    output reg rom_inst_stb;
    output [23:0] rom_inst_addr;
    input [127:0] rom_inst_dout;
    input rom_inst_ack;
    input rom_inst_timeout;

  //--------------------------------------------

  assign ram_inst_stb = 1'b0;
  assign ram_inst_addr[24:0] = 25'h0;

  //--------------------------------------------

  reg line_cnt_inc;
  reg [4:0] line_cnt;

  reg bit_cnt_clr;
  reg bit_cnt_inc;
  reg [6:0] bit_cnt;

  wire crc_bit;
  reg [15:0] crc;

  reg [127:0] line;

  reg [1:0] state;
  reg [1:0] next_state;

  always @(posedge clk) begin
    if (rst) begin
      line_cnt[4:0] <= 5'h0;
    end else begin
      if (~line_cnt[4]) begin
        if (line_cnt_inc) begin
          line_cnt[4:0] <= line_cnt[4:0] + 5'h1;
        end
      end
    end
  end

  assign rom_inst_addr[23:0] = { 20'h0, line_cnt[3:0] };

  always @(posedge clk) begin
    if (bit_cnt_clr) begin
      bit_cnt[6:0] <= 7'h0;
    end else begin
      if (bit_cnt_inc) begin
        bit_cnt[6:0] <= bit_cnt[6:0] + 7'h1;
      end
    end
  end

  assign crc_bit = crc[15] ^ line[127];

  always @(posedge clk) begin
    if (rst) begin
      crc[15:0] <= 16'h0;
    end else begin
      if (bit_cnt_inc) begin
        crc[15] <= crc[14];
        crc[14] <= crc[13];
        crc[13] <= crc[12];
        crc[12] <= crc[11] ^ crc_bit;
        crc[11] <= crc[10];
        crc[10] <= crc[ 9];
        crc[ 9] <= crc[ 8];
        crc[ 8] <= crc[ 7];
        crc[ 7] <= crc[ 6];
        crc[ 6] <= crc[ 5];
        crc[ 5] <= crc[ 4] ^ crc_bit;
        crc[ 4] <= crc[ 3];
        crc[ 3] <= crc[ 2];
        crc[ 2] <= crc[ 1];
        crc[ 1] <= crc[ 0];
        crc[ 0] <= crc_bit;
      end
    end
  end

  assign test_crc_ok = (crc[15:0] == 16'h7E55);

  always @(posedge clk) begin
    if (rom_inst_ack) begin
      line[127:0] <= rom_inst_dout[127:0];
    end else begin
      if (bit_cnt_inc) begin
        line[127:0] <= { line[126:0], line[127] };
      end
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      state[1:0] <= 2'h0;
    end else begin
      state[1:0] <= next_state[1:0];
    end
  end

  always @(*) begin
    case (state[1:0])
      2'h0:
        begin
          rom_inst_stb = 1'b0;
          line_cnt_inc = 1'b0;
          bit_cnt_clr = 1'b0;
          bit_cnt_inc = 1'b0;
          if (~line_cnt[4]) begin
            next_state = 2'h1;
          end else begin
            next_state = 2'h0;
          end
        end
      2'h1:
        begin
          rom_inst_stb = 1'b1;
          line_cnt_inc = 1'b0;
          bit_cnt_clr = 1'b1;
          bit_cnt_inc = 1'b0;
          if (rom_inst_ack) begin
            next_state = 2'h2;
          end else begin
            next_state = 2'h1;
          end
        end
      2'h2:
        begin
          rom_inst_stb = 1'b0;
          line_cnt_inc = 1'b0;
          bit_cnt_clr = 1'b0;
          bit_cnt_inc = 1'b1;
          if (bit_cnt[6:0] == 7'h7F) begin
            next_state = 2'h3;
          end else begin
            next_state = 2'h2;
          end
        end
      2'h3:
        begin
          rom_inst_stb = 1'b0;
          line_cnt_inc = 1'b1;
          bit_cnt_clr = 1'b0;
          bit_cnt_inc = 1'b0;
          next_state = 2'h0;
        end
      default:
        begin
          rom_inst_stb = 1'b0;
          line_cnt_inc = 1'b0;
          bit_cnt_clr = 1'b0;
          bit_cnt_inc = 1'b0;
          next_state = 2'h0;
        end
    endcase
  end

  //--------------------------------------------

endmodule
