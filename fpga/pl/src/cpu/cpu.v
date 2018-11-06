//
// cpu.v -- the ECO32 CPU
//


`timescale 1ns/10ps
`default_nettype none


module cpu(clk, rst,
           temp_out, temp_trg);
    input clk;				// system clock
    input rst;				// system reset
    output [31:0] temp_out;		// temporary output
    output temp_trg;			// temporary trigger

  wire if1b_ready;
  wire if1a_valid;
  wire [9:0] if1a_counter;
  wire if1b_valid;
  wire [31:0] if1b_vaddr;

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
    .if1b_ready_in(1'b1),
    .if1b_valid_out(if1b_valid),
    .if1b_vaddr_out(if1b_vaddr[31:0])
  );

  //--------------------------------------
  // temporary output and trigger
  //--------------------------------------

  wire trigger;
  reg trigger_buf;

  assign temp_out = if1b_vaddr[31:0];

  assign trigger = if1a_valid & (if1a_counter[9:0] == 10'h0FE);

  always @(posedge clk) begin
    if (rst) begin
      trigger_buf <= 1'b0;
    end else begin
      trigger_buf <= trigger;
    end
  end

  assign temp_trg = trigger_buf & if1b_valid;

endmodule
