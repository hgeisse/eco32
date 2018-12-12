//
// if2.v -- instruction fetch, stage 2 (address translation)
//


`timescale 1ns/10ps
`default_nettype none


module if2(clk, rst,
           if2_ready_out, if2_valid_in, if2_vaddr_in,
           if2_ready_in, if2_valid_out, if2_paddr_out);
    input clk;
    input rst;
    //----------------
    output if2_ready_out;
    input if2_valid_in;
    input [31:0] if2_vaddr_in;
    //----------------
    input if2_ready_in;
    output if2_valid_out;
    output [29:0] if2_paddr_out;

  reg valid_buf;

  wire [19:0] page;
  wire [11:0] offset;
  reg [19:0] page_buf;
  reg [11:0] offset_buf;
  wire [19:0] tlb_frame;
  wire [17:0] mapped_frame;
  wire [17:0] direct_frame;
  wire [17:0] frame;

  //--------------------------------------------

  assign if2_ready_out = if2_ready_in;

  always @(posedge clk) begin
    if (rst) begin
      valid_buf <= 1'b0;
    end else begin
      if (if2_ready_out) begin
        valid_buf <= if2_valid_in;
      end
    end
  end

  assign if2_valid_out = valid_buf;

  //--------------------------------------------

  assign page[19:0] = if2_vaddr_in[31:12];
  assign offset[11:0] = if2_vaddr_in[11:0];

  always @(posedge clk) begin
    if (if2_ready_out) begin
      page_buf[19:0] <= page[19:0];
      offset_buf[11:0] <= offset[11:0];
    end
  end

  tlb tlb_1(
    .clk(clk),
    .page_in(page[19:0]),
    .miss(),
    .found(),
    .frame_out(tlb_frame[19:0]),
    .rw_index(5'h0),
    .r_page(),
    .r_frame(),
    .w_enable(1'b0),
    .w_page(20'h0),
    .w_frame(20'h0)
  );

  assign mapped_frame[17:0] = tlb_frame[19:2];
  assign direct_frame[17:0] = page_buf[17:0];

  assign frame[17:0] =
    (page_buf[19] & page_buf[18]) ?
      direct_frame[17:0] : mapped_frame[17:0];

  assign if2_paddr_out[29:0] = { frame[17:0], offset_buf[11:0] };

endmodule


module tlb(clk,
           page_in, miss, found, frame_out,
           rw_index, r_page, r_frame,
           w_enable, w_page, w_frame);
    input clk;
    //----------------
    input [19:0] page_in;
    output miss;
    output [4:0] found;
    output reg [19:0] frame_out;
    //----------------
    input [4:0] rw_index;
    output reg [19:0] r_page;
    output reg [19:0] r_frame;
    input w_enable;
    input [19:0] w_page;
    input [19:0] w_frame;

  reg [19:0] page[0:31];
  reg [19:0] frame[0:31];

  wire [19:0] p00, p01, p02, p03;
  wire [19:0] p04, p05, p06, p07;
  wire [19:0] p08, p09, p10, p11;
  wire [19:0] p12, p13, p14, p15;
  wire [19:0] p16, p17, p18, p19;
  wire [19:0] p20, p21, p22, p23;
  wire [19:0] p24, p25, p26, p27;
  wire [19:0] p28, p29, p30, p31;
  wire [31:0] match;

  initial begin
    #0          $readmemh("page.dat", page);
                $readmemh("frame.dat", frame);
  end

  assign p00 = page[ 0];
  assign p01 = page[ 1];
  assign p02 = page[ 2];
  assign p03 = page[ 3];
  assign p04 = page[ 4];
  assign p05 = page[ 5];
  assign p06 = page[ 6];
  assign p07 = page[ 7];
  assign p08 = page[ 8];
  assign p09 = page[ 9];
  assign p10 = page[10];
  assign p11 = page[11];
  assign p12 = page[12];
  assign p13 = page[13];
  assign p14 = page[14];
  assign p15 = page[15];
  assign p16 = page[16];
  assign p17 = page[17];
  assign p18 = page[18];
  assign p19 = page[19];
  assign p20 = page[20];
  assign p21 = page[21];
  assign p22 = page[22];
  assign p23 = page[23];
  assign p24 = page[24];
  assign p25 = page[25];
  assign p26 = page[26];
  assign p27 = page[27];
  assign p28 = page[28];
  assign p29 = page[29];
  assign p30 = page[30];
  assign p31 = page[31];

  assign match[ 0] = (page_in == p00);
  assign match[ 1] = (page_in == p01);
  assign match[ 2] = (page_in == p02);
  assign match[ 3] = (page_in == p03);
  assign match[ 4] = (page_in == p04);
  assign match[ 5] = (page_in == p05);
  assign match[ 6] = (page_in == p06);
  assign match[ 7] = (page_in == p07);
  assign match[ 8] = (page_in == p08);
  assign match[ 9] = (page_in == p09);
  assign match[10] = (page_in == p10);
  assign match[11] = (page_in == p11);
  assign match[12] = (page_in == p12);
  assign match[13] = (page_in == p13);
  assign match[14] = (page_in == p14);
  assign match[15] = (page_in == p15);
  assign match[16] = (page_in == p16);
  assign match[17] = (page_in == p17);
  assign match[18] = (page_in == p18);
  assign match[19] = (page_in == p19);
  assign match[20] = (page_in == p20);
  assign match[21] = (page_in == p21);
  assign match[22] = (page_in == p22);
  assign match[23] = (page_in == p23);
  assign match[24] = (page_in == p24);
  assign match[25] = (page_in == p25);
  assign match[26] = (page_in == p26);
  assign match[27] = (page_in == p27);
  assign match[28] = (page_in == p28);
  assign match[29] = (page_in == p29);
  assign match[30] = (page_in == p30);
  assign match[31] = (page_in == p31);

  assign miss = ~(| match[31:0]);

  assign found[0] = match[ 1] | match[ 3] | match[ 5] | match[ 7] |
                    match[ 9] | match[11] | match[13] | match[15] |
                    match[17] | match[19] | match[21] | match[23] |
                    match[25] | match[27] | match[29] | match[31];
  assign found[1] = match[ 2] | match[ 3] | match[ 6] | match[ 7] |
                    match[10] | match[11] | match[14] | match[15] |
                    match[18] | match[19] | match[22] | match[23] |
                    match[26] | match[27] | match[30] | match[31];
  assign found[2] = match[ 4] | match[ 5] | match[ 6] | match[ 7] |
                    match[12] | match[13] | match[14] | match[15] |
                    match[20] | match[21] | match[22] | match[23] |
                    match[28] | match[29] | match[30] | match[31];
  assign found[3] = match[ 8] | match[ 9] | match[10] | match[11] |
                    match[12] | match[13] | match[14] | match[15] |
                    match[24] | match[25] | match[26] | match[27] |
                    match[28] | match[29] | match[30] | match[31];
  assign found[4] = match[16] | match[17] | match[18] | match[19] |
                    match[20] | match[21] | match[22] | match[23] |
                    match[24] | match[25] | match[26] | match[27] |
                    match[28] | match[29] | match[30] | match[31];

  always @(posedge clk) begin
    frame_out <= frame[found];
  end

  always @(posedge clk) begin
    if (w_enable) begin
      page[rw_index] <= w_page;
      frame[rw_index] <= w_frame;
    end else begin
      r_page <= page[rw_index];
      r_frame <= frame[rw_index];
    end
  end

endmodule
