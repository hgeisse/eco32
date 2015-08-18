//
// kbd.v -- PS/2 keyboard interface
//


`timescale 1ns/10ps
`default_nettype none


module kbd(clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack, irq);
    input clk;
    input rst;
    input stb;
    input we;
    input addr;
    input [7:0] data_in;
    output [7:0] data_out;
    output ack;
    output irq;

  reg [39:0] kbd_data[0:255];		// space for 256 data requests
  reg [7:0] kbd_data_index;		// next location to read
  wire [39:0] next_full;		// 40 bits from kbd_data
  wire [31:0] next_time;		// 32 bits delta clock ticks
  wire [7:0] next_code;			// 8 bits character code
  reg [31:0] counter;			// delta tick counter
  wire next_rdy;			// delta time expired

  reg [7:0] data;
  reg rdy;
  reg ien;
  reg [7:2] other_bits;

  initial begin
    $readmemh("kbd.dat", kbd_data);
  end

  assign next_full[39:0] = kbd_data[kbd_data_index];
  assign next_time[31:0] = next_full[39:8];
  assign next_code[7:0] = next_full[7:0];

  always @(posedge clk) begin
    if (rst) begin
      kbd_data_index <= 0;
      counter <= 0;
    end else begin
      if (counter == 0) begin
        counter <= next_time;
      end else
      if (counter == 1) begin
        kbd_data_index <= kbd_data_index + 1;
        counter <= counter - 1;
      end else begin
        if (counter != 32'hFFFFFFFF) begin
          counter <= counter - 1;
        end
      end
    end
  end

  assign next_rdy = (counter == 1) ? 1 : 0;

  always @(posedge clk) begin
    if (rst) begin
      data <= 8'h00;
      rdy <= 0;
      ien <= 0;
      other_bits <= 6'b000000;
    end else begin
      if (next_rdy) begin
        data <= next_code;
      end
      if (next_rdy == 1 ||
          (stb == 1 && we == 0 && addr == 1)) begin
        rdy <= next_rdy;
      end
      if (stb == 1 && we == 1 && addr == 0) begin
        rdy <= data_in[0];
        ien <= data_in[1];
        other_bits <= data_in[7:2];
      end
    end
  end

  assign data_out =
    (addr == 0) ? { other_bits[7:2], ien, rdy } : data[7:0];
  assign ack = stb;
  assign irq = ien & rdy;

endmodule
