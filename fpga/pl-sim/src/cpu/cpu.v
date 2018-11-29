//
// cpu.v -- the ECO32 CPU
//


`timescale 1ns/10ps
`default_nettype none


module cpu(clk, rst,
           ram_inst_stb, ram_inst_addr,
           ram_inst_dout, ram_inst_ack,
           ram_inst_timeout,
           rom_inst_stb, rom_inst_addr,
           rom_inst_dout, rom_inst_ack,
           rom_inst_timeout,
           test_step, test_good, test_ended);
    input clk;				// system clock
    input rst;				// system reset
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
    //----------------
    output test_step;			// test step completed
    output test_good;			// test step good
    output test_ended;			// test ended

  wire if1b_ready;
  wire if1a_valid;
  wire [9:0] if1a_counter;
  wire if1b_valid;
  wire [31:0] if1b_vaddr;

  wire if2_ready;
  wire if2_valid;
  wire [29:0] if2_paddr;

  wire if3_ready;
  wire if3_valid;
  wire [31:0] if3_inst;

  wire test_ready;

  //--------------------------------------
  // module instances
  //--------------------------------------

  if1a if1a_1(
    .clk(clk),
    .rst(rst),
    .if1a_ready_in(if1b_ready),
    .if1a_valid_out(if1a_valid),
    .if1a_counter_out(if1a_counter[9:0])
  );

  if1b if1b_1(
    .clk(clk),
    .rst(rst),
    .if1b_ready_out(if1b_ready),
    .if1b_valid_in(if1a_valid),
    .if1b_counter_in(if1a_counter[9:0]),
    .if1b_ready_in(if2_ready),
    .if1b_valid_out(if1b_valid),
    .if1b_vaddr_out(if1b_vaddr[31:0])
  );

  if2 if2_1(
    .clk(clk),
    .rst(rst),
    .if2_ready_out(if2_ready),
    .if2_valid_in(if1b_valid),
    .if2_vaddr_in(if1b_vaddr[31:0]),
    .if2_ready_in(if3_ready),
    .if2_valid_out(if2_valid),
    .if2_paddr_out(if2_paddr[29:0])
  );

  if3 if3_1(
    .clk(clk),
    .rst(rst),
    //----------------
    .if3_ready_out(if3_ready),
    .if3_valid_in(if2_valid),
    .if3_paddr_in(if2_paddr[29:0]),
    .if3_ready_in(test_ready),
    .if3_valid_out(if3_valid),
    .if3_inst_out(if3_inst[31:0]),
    //----------------
    .ram_inst_stb(ram_inst_stb),
    .ram_inst_addr(ram_inst_addr[24:0]),
    .ram_inst_dout(ram_inst_dout[127:0]),
    .ram_inst_ack(ram_inst_ack),
    .ram_inst_timeout(ram_inst_timeout),
    //----------------
    .rom_inst_stb(rom_inst_stb),
    .rom_inst_addr(rom_inst_addr[23:0]),
    .rom_inst_dout(rom_inst_dout[127:0]),
    .rom_inst_ack(rom_inst_ack),
    .rom_inst_timeout(rom_inst_timeout)
  );

  //--------------------------------------
  // test signals
  //--------------------------------------

  test test_1(
    .clk(clk),
    .rst(rst),
    //----------------
    .test_ready_out(test_ready),
    .test_valid_in(if3_valid),
    .test_data_in(if3_inst[31:0]),
    //----------------
    .test_step(test_step),
    .test_good(test_good),
    .test_ended(test_ended)
  );

endmodule
