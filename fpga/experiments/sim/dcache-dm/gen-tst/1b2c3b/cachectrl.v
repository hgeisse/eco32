//
// cachectrl.v -- cache controller
//


`timescale 1ns/10ps
`default_nettype none


module cachectrl(clk, rst,
                 cache_ready_out);
    input clk;
    input rst;
    //----------------
    output reg cache_ready_out;

  initial begin
    #0    cache_ready_out = 0;
    #100  cache_ready_out = 1;
    #815  cache_ready_out = 0;
    #50   cache_ready_out = 1;
  end

endmodule
