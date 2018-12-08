//
// democache.v -- simulate a small demonstration cache
//


`timescale 1ns/10ps
`default_nettype none


module democache;

  reg clk;			// system clock
  reg rst_in;			// reset, input
  reg rst;			// system reset

  wire cache_ready_out;
//  wire cache_valid_in;
//  wire [15:0] cache_addr_in;
//  wire cache_we_in;
//  wire [7:0] cache_data_in;
//  wire cache_ready_in;
//  wire cache_valid_out;
//  wire [7:0] cache_data_out;
//  wire test_ended;
//  wire test_error;

  //
  // simulation control
  //

  initial begin
    #0          $timeformat(-9, 1, " ns", 12);
                $dumpfile("dump.vcd");
                $dumpvars(0, democache);
                clk = 1;
                rst_in = 1;
    #23         rst_in = 0;
    #200000     $finish;
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

  cachetest cachetest_1(
    .clk(clk),
    .rst(rst),
    //----------------
    .ready_in(cache_ready_out)
//    .valid_out(cache_valid_in),
//    .addr_out(cache_addr_in[15:0]),
//    .we_out(cache_we_in),
//    .data_out(cache_data_in[7:0]),
    //----------------
//    .ready_out(cache_ready_in),
//    .valid_in(cache_valid_out),
//    .data_in(cache_data_out[7:0]),
    //----------------
//    .test_ended(test_ended),
//    .test_error(test_error)
  );

  cachectrl cachectrl_1(
    .clk(clk),
    .rst(rst),
    //----------------
    .cache_ready_out(cache_ready_out)
//    .cache_valid_in(cache_valid_in),
//    .cache_addr_in(cache_addr_in[15:0]),
//    .cache_we_in(cache_we_in),
//    .cache_data_in(cache_data_in[7:0]),
    //----------------
//    .cache_ready_in(cache_ready_in),
//    .cache_valid_out(cache_valid_out),
//    .cache_data_out(cache_data_out[7:0])
    //----------------
  );

endmodule
