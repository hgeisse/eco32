module kbd(ps2_clk, ps2_data,
           clk, reset,
           en, wr, addr2,
           data_in, data_out,
           wt, irq);
    input ps2_clk;
    input ps2_data;
    input clk;
    input reset;
    input en;
    input wr;
    input addr2;
    input [7:0] data_in;
    output [7:0] data_out;
    output wt;
    output irq;

  wire [7:0] keyboard_data;
  wire keyboard_rdy;
  reg [7:0] data;
  reg rdy;
  reg ien;

  keyboard keyboard1(
    .ps2_clk(ps2_clk),
    .ps2_data(ps2_data),
    .clk(clk),
    .reset(reset),
    .keyboard_data(keyboard_data[7:0]),
    .keyboard_rdy(keyboard_rdy)
  );

  always @(posedge clk) begin
    if (reset == 1) begin
      data <= 8'h00;
      rdy <= 0;
      ien <= 0;
    end else begin
      if (keyboard_rdy == 1) begin
        data <= keyboard_data;
      end
      if (keyboard_rdy == 1 ||
          (en == 1 && wr == 0 && addr2 == 1)) begin
        rdy <= keyboard_rdy;
      end
      if (en == 1 && wr == 1 && addr2 == 0) begin
        rdy <= data_in[0];
        ien <= data_in[1];
      end
    end
  end

  assign data_out =
    (addr2 == 0) ? { 6'b000000, ien, rdy } : data;
  assign wt = 1'b0;
  assign irq = ien & rdy;

endmodule
