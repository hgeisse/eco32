//
// bio.v -- board specific I/O
//


module bio(clk, reset,
           en, wr, addr,
           data_in, data_out,
           wt,
           sw1_3_n, sw1_4_n,
           sw2_n, sw3_n);
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
    input sw1_3_n;
    input sw1_4_n;
    input sw2_n;
    input sw3_n;

  reg [31:0] bio_out;
  wire [31:0] bio_in;

  reg sw1_3_p_n;
  reg sw1_3_s_n;
  reg sw1_4_p_n;
  reg sw1_4_s_n;
  reg sw2_p_n;
  reg sw2_s_n;
  reg sw3_p_n;
  reg sw3_s_n;

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

  always @(posedge clk) begin
    sw1_3_p_n <= sw1_3_n;
    sw1_3_s_n <= sw1_3_p_n;
    sw1_4_p_n <= sw1_4_n;
    sw1_4_s_n <= sw1_4_p_n;
    sw2_p_n <= sw2_n;
    sw2_s_n <= sw2_p_n;
    sw3_p_n <= sw3_n;
    sw3_s_n <= sw3_p_n;
  end

  assign bio_in[31:0] =
    { 28'h0, ~sw1_3_s_n, ~sw1_4_s_n, ~sw2_s_n, ~sw3_s_n };

endmodule
