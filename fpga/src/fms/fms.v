//
// fms.v -- FM synthesizer
//
// NOTE: this is a fake module for now
//


module fms(clk, reset,
           en, wr, addr,
           data_in, data_out,
           wt,
           next, sample_l, sample_r);
    // internal interface
    input clk;
    input reset;
    input en;
    input wr;
    input [11:2] addr;
    input [31:0] data_in;
    output [31:0] data_out;
    output wt;
    // DAC controller interface
    input next;
    output [15:0] sample_l;
    output [15:0] sample_r;

  reg [31:0] value;

  always @(posedge clk) begin
    if (reset) begin
      value[31:0] <= 32'h0;
    end else begin
      if (en & wr & ~|addr[11:2]) begin
        value[31:0] <= data_in[31:0];
      end
    end
  end

  assign data_out[31:0] = value[31:0];
  assign wt = 0;

  assign sample_l[15:0] = value[31:16];
  assign sample_r[15:0] = value[15:0];

endmodule
