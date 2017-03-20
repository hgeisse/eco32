//
// sdcard.v -- SD card simulation model
//


`timescale 1ns/10ps
`default_nettype none


module sdcard(cs_n, sclk, di, do);
    input cs_n;
    input sclk;
    input di;
    output do;

  reg [8:0] sreg;

  initial begin
    sreg = 9'h1FF;
  end

  always @(posedge sclk) begin
    if (~cs_n) begin
      sreg[0] <= di;
    end
  end

  always @(negedge sclk) begin
    if (~cs_n) begin
      sreg[8:1] <= sreg[7:0];
    end
  end

  assign do = cs_n ? 1'bz : sreg[8];

endmodule
