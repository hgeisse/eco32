//
// if1b.v -- instruction fetch, stage 1b (read vaddr[counter])
//


`timescale 1ns/10ps
`default_nettype none


module if1b(clk, rst,
            if1b_ready_out, if1b_valid_in, if1b_counter_in,
            if1b_ready_in, if1b_valid_out, if1b_vaddr_out);
    input clk;
    input rst;
    //----------------
    output if1b_ready_out;
    input if1b_valid_in;
    input [9:0] if1b_counter_in;
    //----------------
    input if1b_ready_in;
    output if1b_valid_out;
    output [31:0] if1b_vaddr_out;

  reg valid_buf;

  reg [31:0] vaddr_gen[0:1023];
  reg [31:0] vaddr_gen_out;

  //--------------------------------------------

  assign if1b_ready_out = if1b_ready_in;

  always @(posedge clk) begin
    if (rst) begin
      valid_buf <= 1'b0;
    end else begin
      if (if1b_ready_out) begin
        valid_buf <= if1b_valid_in;
      end
    end
  end

  assign if1b_valid_out = valid_buf;

  //--------------------------------------------

  initial begin
    #0          $readmemh("vaddr.dat", vaddr_gen);
  end

  always @(posedge clk) begin
    if (if1b_ready_out) begin
      vaddr_gen_out <= vaddr_gen[if1b_counter_in];
    end
  end

  assign if1b_vaddr_out[31:0] =
    valid_buf ? vaddr_gen_out[31:0] : 32'hxxxxxxxx;

endmodule
