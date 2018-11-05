//
// eco32test.v -- test bench for ECO32
//


`timescale 1ns/10ps
`default_nettype none


module eco32test;

  integer fd;				// file descriptor
  integer fr;				// result of file operation
  time duration;			// duration of simulation

  reg clk_in;				// clock, input, 50 MHz
  reg rst_in_n;				// reset, input, active low

  //
  // simulation control
  //
  initial begin
    #0          $dumpfile("dump.vcd");
                $dumpvars(0, eco32test);
                fd = $fopen("duration.dat", "r");
                fr = $fscanf(fd, "%d", duration);
                $fclose(fd);
                clk_in = 1;
                rst_in_n = 0;
    #93         rst_in_n = 1;
    #duration   $finish;
  end

  //
  // clock generator
  //
  always begin
    #10 clk_in = ~clk_in;		// 20 nsec cycle time
  end

  //
  // create an instance of ECO32
  //
  eco32 eco32_1(
    .clk_in(clk_in),
    .rst_in_n(rst_in_n)
  );

endmodule
