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

  initial begin
    $readmemh("kbd.dat", kbd_data);
  end

  assign next_full[39:0] = kbd_data[kbd_data_index];
  assign next_time[31:0] = next_full[39:8];
  assign next_code[7:0] = next_full[7:0];

  always @(posedge clk) begin
    if (rst) begin
      kbd_data_index[7:0] <= 8'd0;
      counter[31:0] <= 32'd0;
    end else begin
      if (counter[31:0] == 32'd0) begin
        counter[31:0] <= next_time[31:0];
      end else
      if (counter[31:0] == 32'd1) begin
        kbd_data_index[7:0] <= kbd_data_index[7:0] + 8'd1;
        counter[31:0] <= counter[31:0] - 32'd1;
      end else begin
        if (counter[31:0] != 32'hFFFFFFFF) begin
          counter[31:0] <= counter[31:0] - 32'd1;
        end
      end
    end
  end

  assign next_rdy = (counter[31:0] == 32'd1) ? 1'b1 : 1'b0;

  always @(posedge clk) begin
    if (rst) begin
      data <= 8'h00;
      rdy <= 1'b0;
      ien <= 1'b0;
    end else begin
      if (next_rdy) begin
        data[7:0] <= next_code[7:0];
      end
      if (next_rdy | (stb & ~we & addr)) begin
        rdy <= next_rdy;
      end
      if (stb & we & ~addr) begin
        ien <= data_in[1];
      end
    end
  end

  assign data_out = ~addr ? { 6'h00, ien, rdy } : data[7:0];
  assign ack = stb;
  assign irq = ien & rdy;

endmodule
