//
// pipetest.v -- test the pipeline
//


`timescale 1ns/10ps
`default_nettype none


module pipetest;

  reg clk;			// system clock
  reg rst_in;			// reset, input
  reg rst;			// system reset

  wire test_ended;		// test has ended
  wire test_error;		// test has failed

  //
  // simulation control
  //

  initial begin
    #0          $timeformat(-9, 1, " ns", 12);
                $dumpfile("dump.vcd");
                $dumpvars(0, pipetest);
                clk = 1;
                rst_in = 1;
    #23         rst_in = 0;
    #6000       $finish;
  end

  //
  // clock generator
  //

  always begin
    #5 clk = ~clk;		// 10 nsec cycle time
  end

  //
  // reset synchronizer
  //

  always @(posedge clk) begin
    rst <= rst_in;
  end

  //
  // module instantiations
  //

  pipe pipe_1(
    .clk(clk),
    .rst(rst),
    .test_ended(test_ended),
    .test_error(test_error)
  );

endmodule
