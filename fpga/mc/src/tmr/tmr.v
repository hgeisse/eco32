//
// tmr.v -- programmable timer
//


`timescale 1ns/10ps
`default_nettype none


module tmr(clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack, irq);
    input clk;
    input rst;
    input stb;
    input we;
    input [3:2] addr;
    input [31:0] data_in;
    output reg [31:0] data_out;
    output ack;
    output irq;

  reg [31:0] counter;
  reg [31:0] divisor;
  reg divisor_loaded;
  reg expired;
  reg exp;
  reg ien;

  always @(posedge clk) begin
    if (divisor_loaded) begin
      counter[31:0] <= divisor[31:0];
      expired <= 1'b0;
    end else begin
      if (counter[31:0] == 32'h00000001) begin
        counter[31:0] <= divisor[31:0];
        expired <= 1'b1;
      end else begin
        counter[31:0] <= counter[31:0] - 32'h00000001;
        expired <= 1'b0;
      end
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      divisor[31:0] <= 32'hFFFFFFFF;
      divisor_loaded <= 1'b1;
      exp <= 1'b0;
      ien <= 1'b0;
    end else begin
      if (expired) begin
        exp <= 1'b1;
      end else begin
        if (stb == 1'b1 && we == 1'b0 && addr[3:2] == 2'b00) begin
          // read ctrl
          exp <= 1'b0;
        end
        if (stb == 1'b1 && we == 1'b1 && addr[3:2] == 2'b00) begin
          // write ctrl
          ien <= data_in[1];
        end
        if (stb == 1'b1 && we == 1'b1 && addr[3:2] == 2'b01) begin
          // write divisor
          divisor[31:0] <= data_in[31:0];
          divisor_loaded <= 1'b1;
        end else begin
          divisor_loaded <= 1'b0;
        end
      end
    end
  end

  always @(*) begin
    case (addr[3:2])
      2'b00:
        // ctrl
        data_out[31:0] = { 30'h00000000, ien, exp };
      2'b01:
        // divisor
        data_out[31:0] = divisor[31:0];
      2'b10:
        // counter
        data_out[31:0] = counter[31:0];
      2'b11:
        // not used
        data_out[31:0] = 32'hxxxxxxxx;
      default:
        data_out[31:0] = 32'hxxxxxxxx;
    endcase
  end

  assign ack = stb;
  assign irq = ien & exp;

endmodule
