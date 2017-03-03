//
// memdelay.v -- test memory delay
//


`timescale 1ns/10ps
`default_nettype none


module memdelay;

  reg clk;
  reg rst_in;
  reg rst;
  reg stb;
  reg we;
  wire [22:0] addr;
  wire [31:0] data_in;
  wire [31:0] data_out;
  wire ack;
  reg [5:0] state;
  reg [5:0] next_state;

  initial begin
    #0     $dumpfile("dump.vcd");
           $dumpvars(0, memdelay);
           clk = 1;
           rst_in = 1;
    #25    rst_in = 0;
    #1000  $finish;
  end

  always begin
    #10 clk = ~ clk;
  end

  always @(posedge clk) begin
    rst <= rst_in;
  end

  assign addr[22:0] = 23'h123456;
  assign data_in[31:0] = 32'h12345678;

  ram ram_1(
    .clk(clk),
    .rst(rst),
    .stb(stb),
    .we(we),
    .addr(addr[22:0]),
    .data_in(data_in[31:0]),
    .data_out(data_out[31:0]),
    .ack(ack)
  );

  always @(posedge clk) begin
    if (rst) begin
      state <= 0;
    end else begin
      state <= next_state;
    end
  end

  always @(*) begin
    case (state[5:0])
      6'h0:
        begin
          stb = 0;
          we = 0;
          next_state = 6'h1;
        end
      6'h1:
        begin
          stb = 0;
          we = 0;
          next_state = 6'h2;
        end
      6'h2:
        begin
          stb = 1;
          we = 0;
          if (ack) begin
            next_state = 6'h3;
          end else begin
            next_state = 6'h2;
          end
        end
      6'h3:
        begin
          stb = 0;
          we = 0;
          next_state = 6'h4;
        end
      6'h4:
        begin
          stb = 0;
          we = 0;
          next_state = 6'h5;
        end
      6'h5:
        begin
          stb = 1;
          we = 1;
          if (ack) begin
            next_state = 6'h6;
          end else begin
            next_state = 6'h5;
          end
        end
      6'h6:
        begin
          stb = 0;
          we = 0;
          next_state = 6'h6;
        end
      default:
        begin
          stb = 0;
          we = 0;
          next_state = 6'h0;
        end
    endcase
  end

endmodule
