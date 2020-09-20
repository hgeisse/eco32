//
// i2c_tst.v -- test bench for the I2C master controller
//


`timescale 1ns/10ps
`default_nettype none


module i2c_tst;

  reg clk;			// system clock
  reg rst_in;			// reset, input
  reg rst;			// system reset

  wire i2c_scl;			// I2C clock line
  wire i2c_sda;			// I2C data line

  reg [15:0] data;		// data to be transmitted
  reg start;			// start transmission
  wire done;			// transmission done
  wire error;			// transmission failed

  //
  // simulation control
  //

  initial begin
    #0        $timeformat(-9, 1, " ns", 12);
              $dumpfile("dump.vcd");
              $dumpvars(0, i2c_tst);
              clk = 1;
              rst_in = 1;
              data[15:0] = 16'hxxxx;
              start = 0;
    #73       rst_in = 0;
    #211      data[15:0] = 16'hDEAD;
              start = 1;
    #20       start = 0;
    #100000   $finish;
  end

  //
  // clock generator
  //

  always begin
    #10 clk = ~ clk;		// 20 nsec cycle time
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

  pullup(i2c_scl);
  pullup(i2c_sda);

  i2c_mc i2c_mc_0(
    .clk(clk),
    .rst(rst),
    .data(data[15:0]),
    .start(start),
    .done(done),
    .error(error),
    .i2c_scl(i2c_scl),
    .i2c_sda(i2c_sda)
  );

  ack_gen ack_gen_0(
    .i2c_sda(i2c_sda)
  );

endmodule


module ack_gen(i2c_sda);
    inout i2c_sda;

  reg ack;

  initial begin
    #0        ack = 0;
    #28600    ack = 1;
    #2000     ack = 0;
    #25740    ack = 1;
    #2000     ack = 0;
    #25740    ack = 1;
    #2000     ack = 0;
  end

  assign i2c_sda = ack ? 1'b0 : 1'bz;

endmodule
