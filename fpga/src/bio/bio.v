//
// bio.v -- board specific I/O
//


module bio(clk, reset,
           en, wr, addr,
           data_in, data_out,
           wt);
    // internal interface
    input clk;
    input reset;
    input en;
    input wr;
    input addr;
    input [31:0] data_in;
    output [31:0] data_out;
    output wt;
    // external interface

  reg [31:0] bio_out;
  wire [31:0] bio_in;

  always @(posedge clk) begin
    if (reset) begin
      bio_out[31:0] <= 32'h0;
    end else begin
      if (en & wr & ~addr) begin
        bio_out[31:0] <= data_in[31:0];
      end
    end
  end

  assign data_out[31:0] =
    (addr == 0) ? bio_out[31:0] : bio_in[31:0];
  assign wt = 0;

  assign bio_in[31:0] = { 28'h0, 4'h0 };

endmodule
