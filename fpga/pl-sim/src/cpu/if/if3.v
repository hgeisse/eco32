//
// if3.v -- instruction fetch, stage 3 (icache)
//


`timescale 1ns/10ps
`default_nettype none


//
// Geometry:
//     word-addressed
//     total capacity 16 KB (4 KW)
//     line size 16 bytes (4 words)
//     1024 lines
//     2-way associative
//     512 sets
//


module if3(clk, rst,
           if3_ready_out, if3_valid_in, if3_paddr_in,
           if3_ready_in, if3_valid_out, if3_inst_out,
           ram_inst_stb, ram_inst_addr, ram_inst_dout,
           ram_inst_ack, ram_inst_timeout,
           rom_inst_stb, rom_inst_addr, rom_inst_dout,
           rom_inst_ack, rom_inst_timeout);
    input clk;
    input rst;
    //----------------
    output if3_ready_out;
    input if3_valid_in;
    input [29:0] if3_paddr_in;
    //----------------
    input if3_ready_in;
    output if3_valid_out;
    output [31:0] if3_inst_out;
    //----------------
    output ram_inst_stb;
    output [24:0] ram_inst_addr;
    input [127:0] ram_inst_dout;
    input ram_inst_ack;
    input ram_inst_timeout;
    //----------------
    output rom_inst_stb;
    output [23:0] rom_inst_addr;
    input [127:0] rom_inst_dout;
    input rom_inst_ack;
    input rom_inst_timeout;

  reg valid_buf;

  wire [16:0] tag;
  wire [8:0] index;
  wire [1:0] offset;

  reg [16:0] tag_buf;
  reg [8:0] index_buf;
  reg [1:0] offset_buf;

  wire [1:0] sel_index;
  reg [8:0] cur_index;

  wire set_read;

  reg line_0_valid[0:511];
  reg line_0_valid_out;
  reg [16:0] line_0_tag[0:511];
  reg [16:0] line_0_tag_out;
  reg [127:0] line_0_data[0:511];
  reg [127:0] line_0_data_out;
  wire line_0_hit;

  reg line_1_valid[0:511];
  reg line_1_valid_out;
  reg [16:0] line_1_tag[0:511];
  reg [16:0] line_1_tag_out;
  reg [127:0] line_1_data[0:511];
  reg [127:0] line_1_data_out;
  wire line_1_hit;

  wire [127:0] line_data_out;

  wire hit;

  wire [8:0] lru_index;
  reg lru[0:511];
  reg lru_out;

  wire [127:0] memory_data;
  wire memory_ack;
  reg memory_ack_buf;

  //--------------------------------------------

  assign if3_ready_out =
    if3_ready_in & (hit | ~valid_buf) & ~invalidate;

  always @(posedge clk) begin
    if (rst) begin
      valid_buf <= 1'b0;
    end else begin
      if (if3_ready_out) begin
        valid_buf <= if3_valid_in;
      end
    end
  end

  assign if3_valid_out = valid_buf & hit;

  //--------------------------------------------

  assign tag[16:0] = if3_paddr_in[29:13];
  assign index[8:0] = if3_paddr_in[12:4];
  assign offset[1:0] = if3_paddr_in[3:2];

  always @(posedge clk) begin
    if (if3_ready_out) begin
      tag_buf[16:0] <= tag[16:0];
      index_buf[8:0] <= index[8:0];
      offset_buf[1:0] <= offset[1:0];
    end
  end

  always @(*) begin
    case (sel_index[1:0])
      2'b00: cur_index[8:0] = index[8:0];
      2'b01: cur_index[8:0] = index_buf[8:0];
      2'b10: cur_index[8:0] = index_count[8:0];
      2'b11: cur_index[8:0] = index_count[8:0];
    endcase
  end

  assign set_read = if3_ready_out | memory_ack_buf;

  always @(posedge clk) begin
    if (set_read) begin
      line_0_valid_out <= line_0_valid[cur_index];
      line_0_tag_out <= line_0_tag[cur_index];
      line_0_data_out <= line_0_data[cur_index];
    end
    if ((memory_ack & ~lru_out) | invalidate) begin
      line_0_valid[cur_index] <= ~invalidate;
      line_0_tag[cur_index] <= tag_buf;
      line_0_data[cur_index] <= memory_data;
    end
  end

  assign line_0_hit =
    line_0_valid_out & (line_0_tag_out[16:0] == tag_buf[16:0]);

  always @(posedge clk) begin
    if (set_read) begin
      line_1_valid_out <= line_1_valid[cur_index];
      line_1_tag_out <= line_1_tag[cur_index];
      line_1_data_out <= line_1_data[cur_index];
    end
    if ((memory_ack & lru_out) | invalidate) begin
      line_1_valid[cur_index] <= ~invalidate;
      line_1_tag[cur_index] <= tag_buf;
      line_1_data[cur_index] <= memory_data;
    end
  end

  assign line_1_hit =
    line_1_valid_out & (line_1_tag_out[16:0] == tag_buf[16:0]);

  assign line_data_out[127:0] =
    line_1_hit ? line_1_data_out[127:0] : line_0_data_out[127:0];

  assign if3_inst_out[31:0] =
    ~offset_buf[1] ?
      (~offset_buf[0] ? line_data_out[127:96] : line_data_out[95:64]) :
      (~offset_buf[0] ? line_data_out[ 63:32] : line_data_out[31: 0]);

  //--------------------------------------------

  assign hit = line_0_hit | line_1_hit;

  assign lru_index[8:0] =
    invalidate ? index_count[8:0] : index_buf[8:0];

  //
  // Note: If read address = write address in the same clock cycle,
  //       the LRU memory must read the same value that just gets
  //       written to the common address (and not the old contents).
  //       This is modeled here with blocking assignments. Check
  //       that your synthesizer translates this behavior correctly!
  //
  always @(posedge clk) begin
    if (if3_valid_out | invalidate) begin
      lru[lru_index] = line_0_hit & ~invalidate;
    end
    if (set_read) begin
      lru_out = lru[cur_index];
    end
  end

  //--------------------------------------------

  always @(posedge clk) begin
    memory_ack_buf <= memory_ack;
  end

  assign sel_index[1:0] = { invalidate, memory_ack | memory_ack_buf };

  //--------------------------------------------

  assign ram_inst_stb = 1'b0;
  assign ram_inst_addr[24:0] = 25'h0;

  //--------------------------------------------

  assign rom_inst_stb = ~hit & valid_buf & ~memory_ack_buf;
  assign rom_inst_addr[23:0] = { tag_buf[14:0], index_buf[8:0] };
  assign memory_data[127:0] = rom_inst_dout[127:0];
  assign memory_ack = rom_inst_ack;

  //--------------------------------------------

  reg [9:0] index_count;
  wire invalidate;

  always @(posedge clk) begin
    if (rst) begin
      index_count[9:0] <= 10'd0;
    end else begin
      if (~index_count[9]) begin
        index_count[9:0] <= index_count[9:0] + 10'd1;
      end
    end
  end

  assign invalidate = ~index_count[9];

endmodule
