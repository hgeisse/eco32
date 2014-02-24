//
// tmr.v -- programmable timer
//


module tmr(clk, reset,
           en, wr, addr,
           data_in, data_out,
           wt, irq);
    input clk;
    input reset;
    input en;
    input wr;
    input addr;
    input [31:0] data_in;
    output [31:0] data_out;
    output wt;
    output irq;

  reg [5:0] prescaler;
  reg tick;
  reg [31:0] counter;
  reg [31:0] divisor;
  reg divisor_loaded;
  reg expired;
  reg alarm;
  reg ien;

  always @(posedge clk) begin
    if (reset == 1) begin
      prescaler <= 6'd50;
      tick <= 0;
    end else begin
      if (prescaler == 6'd1) begin
        prescaler <= 6'd50;
        tick <= 1;
      end else begin
        prescaler <= prescaler - 1;
        tick <= 0;
      end
    end
  end

  always @(posedge clk) begin
    if (divisor_loaded == 1) begin
      counter <= divisor;
      expired <= 0;
    end else begin
      if (tick == 1) begin
        if (counter == 32'h00000001) begin
          counter <= divisor;
          expired <= 1;
        end else begin
          counter <= counter - 1;
          expired <= 0;
        end
      end else begin
        expired <= 0;
      end
    end
  end

  always @(posedge clk) begin
    if (reset == 1) begin
      divisor <= 32'hFFFFFFFF;
      divisor_loaded <= 1;
      alarm <= 0;
      ien <= 0;
    end else begin
      if (expired == 1) begin
        alarm <= 1;
      end else begin
        if (en == 1 && wr == 1 && addr == 0) begin
          alarm <= data_in[0];
          ien <= data_in[1];
        end
        if (en == 1 && wr == 1 && addr == 1) begin
          divisor <= data_in;
          divisor_loaded <= 1;
        end else begin
          divisor_loaded <= 0;
        end
      end
    end
  end

  assign data_out =
    (addr == 0) ? { 28'h0000000, 2'b00, ien, alarm } :
                   divisor;
  assign wt = 0;
  assign irq = ien & alarm;

endmodule
