//
// sdcbench.v -- SD card test bench
//


`include "sdctest.v"
`include "sdc.v"
`include "sdcard.v"


`timescale 1ns/10ps
`default_nettype none


module sdcbench;

  reg clk;
  reg rst_in;
  reg rst;

  wire stb;
  wire we;
  wire [3:2] addr;
  wire [31:0] test_out;
  wire [31:0] test_in;
  wire ack;

  wire ss_n;
  wire sclk;
  wire mosi;
  wire miso;
  wire wr_protect;

  initial begin
    #0       $dumpfile("dump.vcd");
             $dumpvars(0, sdcbench);
             clk = 1;
             rst_in = 1;
    #25      rst_in = 0;
    #100000  $finish;
  end

  always begin
    #10 clk = ~ clk;
  end

  always @(posedge clk) begin
    rst <= rst_in;
  end

  sdctest sdctest_1(
    .clk(clk),
    .rst(rst),
    .stb(stb),
    .we(we),
    .addr(addr[3:2]),
    .dout(test_out[31:0]),
    .din(test_in[31:0]),
    .ack(ack)
  );

  sdc sdc_1(
    .clk(clk),
    .rst(rst),
    .stb(stb),
    .we(we),
    .addr(addr[3:2]),
    .data_in(test_out[31:0]),
    .data_out(test_in[31:0]),
    .ack(ack),
    .ss_n(ss_n),
    .sclk(sclk),
    .mosi(mosi),
    .miso(miso),
    .wr_protect(wr_protect)
  );

  sdcard sdcard_1(
    .cs_n(ss_n),
    .sclk(sclk),
    .di(mosi),
    .do(miso),
    .wp(wr_protect)
  );

endmodule
