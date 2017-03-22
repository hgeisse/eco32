//
// sdctest.v -- test circuit for SD card controller
//


`timescale 1ns/10ps
`default_nettype none


module sdctest(clk, rst,
               stb, we, addr,
               dout, din,
               ack);
    input clk;
    input rst;
    output reg stb;
    output reg we;
    output reg addr;
    output reg [31:0] dout;
    input [31:0] din;
    input ack;

  reg [4:0] state;
  reg [5:0] count;
  reg [31:0] data;

  always @(posedge clk) begin
    if (rst) begin
      state <= 0;
      count <= 10;
      stb   <= 0;
      we    <= 0;
      addr  <= 0;
      dout  <= 0;
    end else begin
      if (count != 0) begin
        count <= count - 1;
      end else begin
        case (state)
          5'd0:
            // set clock speed, select slave
            begin
              state <= 1;
              count <= 0;
              stb   <= 1;
              we    <= 1;
              addr  <= 0;
              dout  <= 32'h00000003;  // use this for fast SCLK
//              dout  <= 32'h00000001;  // use this for slow SCLK
            end
          5'd1:
            // done
            begin
              state <= 2;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
            end
          5'd2:
            // send 0xB1
            begin
              state <= 3;
              count <= 0;
              stb   <= 1;
              we    <= 1;
              addr  <= 1;
              dout  <= 32'h000000B1;
            end
          5'd3:
            // done
            begin
              state <= 4;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
            end
          5'd4:
            // read status
            begin
              state <= 5;
              count <= 0;
              stb   <= 1;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
            end
          5'd5:
            // done
            begin
              state <= 6;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
              data[31:0] <= din[31:0];
            end
          5'd6:
            // check ready
            begin
              if (~data[0]) begin
                state <= 4;
              end else begin
                state <= 7;
              end
            end
          5'd7:
            // read data
            begin
              state <= 8;
              count <= 0;
              stb   <= 1;
              we    <= 0;
              addr  <= 1;
              dout  <= 0;
            end
          5'd8:
            // done
            begin
              state <= 9;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
              data[31:0] <= din[31:0];
            end
          5'd9:
            // send 0xFF
            begin
              state <= 10;
              count <= 0;
              stb   <= 1;
              we    <= 1;
              addr  <= 1;
              dout  <= 32'h000000FF;
            end
          5'd10:
            // done
            begin
              state <= 11;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
            end
          5'd11:
            // read status
            begin
              state <= 12;
              count <= 0;
              stb   <= 1;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
            end
          5'd12:
            // done
            begin
              state <= 13;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
              data[31:0] <= din[31:0];
            end
          5'd13:
            // check ready
            begin
              if (~data[0]) begin
                state <= 11;
              end else begin
                state <= 14;
              end
            end
          5'd14:
            // read data
            begin
              state <= 15;
              count <= 0;
              stb   <= 1;
              we    <= 0;
              addr  <= 1;
              dout  <= 0;
            end
          5'd15:
            // done
            begin
              state <= 31;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
              data[31:0] <= din[31:0];
            end
          5'd31:
            // halt
            begin
              state <= 31;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
            end
          default:
            begin
              state <= 0;
              count <= 0;
              stb   <= 0;
              we    <= 0;
              addr  <= 0;
              dout  <= 0;
            end
        endcase
      end
    end
  end

endmodule
