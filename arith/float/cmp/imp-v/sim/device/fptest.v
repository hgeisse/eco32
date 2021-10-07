//
// fptest.v -- toplevel floating-point test module
//


`timescale 1ns / 1ps
`default_nettype none


module fptest;

  reg clk;
  reg run;
  wire stall;
  reg [1:0] pred;
  reg [31:0] x;
  reg [31:0] y;
  wire z;
  wire [4:0] flags;

  //
  // simulation control
  //

  initial begin
    #0    $timeformat(-9, 1, " ns", 12);
`ifdef GENDUMP
          $dumpfile("dump.vcd");
          $dumpvars(0, fptest);
`endif
          clk = 0;
          run = 0;
    #10   clk = 1;
    #10   clk = 0;
          while ($fscanf('h80000000, "%h %h %h", pred, x, y) == 3) begin
            run = 1;
            #10   clk = 1;
            #10   clk = 0;
            while (stall == 1) begin
              #10   clk = 1;
              #10   clk = 0;
            end
            $fdisplay('h80000001, "%h %h", z, flags);
            $fflush('h80000001);
          end
    #10   clk = 1;
    #10   clk = 0;
          $finish;
  end

  //
  // module instantiation
  //

  fpcmp fpcmp_1(
    .clk(clk),
    .run(run),
    .stall(stall),
    .pred(pred[1:0]),
    .x(x[31:0]),
    .y(y[31:0]),
    .z(z),
    .flags(flags[4:0])
  );

endmodule
