//
// fptest.v -- toplevel floating-point test module
//


`timescale 1ns / 1ps
`default_nettype none


module fptest;

  reg clk;
  reg run;
  wire stall;
  reg [1:0] rnd;
  reg [31:0] x;
  wire [31:0] z;
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
          while ($fscanf('h80000000, "%h %h", rnd, x) == 2) begin
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

  fpcif fpcif_1(
    .clk(clk),
    .run(run),
    .stall(stall),
    .rnd(rnd[1:0]),
    .x(x[31:0]),
    .z(z[31:0]),
    .flags(flags[4:0])
  );

endmodule
