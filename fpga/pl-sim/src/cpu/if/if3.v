//
// if3.v -- instruction fetch, stage 3 (icache)
//


`timescale 1ns/10ps
`default_nettype none


module if3(clk, rst,
           ram_inst_stb, ram_inst_addr,
           ram_inst_dout, ram_inst_ack,
           ram_inst_timeout,
           rom_inst_stb, rom_inst_addr,
           rom_inst_dout, rom_inst_ack,
           rom_inst_timeout);
    input clk;
    input rst;
    //----------------
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

  //--------------------------------------------

  assign ram_inst_stb = 1'b0;
  assign ram_inst_addr[24:0] = 25'h0;

  //--------------------------------------------

  assign rom_inst_stb = 1'b0;
  assign rom_inst_addr[23:0] = 24'h0;

  //--------------------------------------------

endmodule
