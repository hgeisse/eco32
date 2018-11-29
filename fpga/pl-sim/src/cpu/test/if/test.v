//
// test.v -- compute CRC fingerprint for IF pipeline stage
//


`timescale 1ns/10ps
`default_nettype none


module test(clk, rst,
            test_ready_out, test_valid_in, test_data_in,
            test_step, test_good, test_ended);
    input clk;
    input rst;
    //----------------
    output reg test_ready_out;
    input test_valid_in;
    input [31:0] test_data_in;
    //----------------
    output test_step;
    output test_good;
    output test_ended;

  //--------------------------------------------

  reg [31:0] crctbl[0:255];
  reg [31:0] crctbl_out;
  reg [31:0] crc32;
  wire [31:0] crc32_new;
  reg [2:0] state;
  reg [2:0] next_state;
  wire [7:0] byte;
  wire [7:0] ctrl;
  reg [1:0] sel;
  reg crc32_we;

  initial begin
    #0          $readmemh("src/cpu/test/if/crctbl.dat", crctbl);
  end

  assign byte[7:0] =
    ~sel[1] ?
      (~sel[0] ? test_data_in[31:24] : test_data_in[23:16]) :
      (~sel[0] ? test_data_in[15: 8] : test_data_in[ 7: 0]);

  assign ctrl[7:0] =
    (crc32_we ? crc32_new[31:24] : crc32[31:24]) ^ byte[7:0];

  always @(posedge clk) begin
    crctbl_out <= crctbl[ctrl];
  end

  assign crc32_new[31:0] = { crc32[23:0], 8'h00 } ^ crctbl_out[31:0];

  always @(posedge clk) begin
    if (rst) begin
      crc32[31:0] <= 32'h0;
    end else begin
      if (crc32_we) begin
        crc32[31:0] <= crc32_new[31:0];
      end
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      state[2:0] <= 3'd0;
    end else begin
      state[2:0] <= next_state[2:0];
    end
  end

  always @(*) begin
    case (state[2:0])
      3'd0:
        begin
          sel[1:0] = 2'b00;
          crc32_we = 1'b0;
          if (test_valid_in) begin
            test_ready_out = 1'b0;
            next_state[2:0] = 3'd1;
          end else begin
            test_ready_out = 1'b1;
            next_state[2:0] = 3'd0;
          end
        end
      3'd1:
        begin
          sel[1:0] = 2'b01;
          crc32_we = 1'b1;
          test_ready_out = 1'b0;
          next_state[2:0] = 3'd2;
        end
      3'd2:
        begin
          sel[1:0] = 2'b10;
          crc32_we = 1'b1;
          test_ready_out = 1'b0;
          next_state[2:0] = 3'd3;
        end
      3'd3:
        begin
          sel[1:0] = 2'b11;
          crc32_we = 1'b1;
          test_ready_out = 1'b0;
          next_state[2:0] = 3'd4;
        end
      3'd4:
        begin
          sel[1:0] = 2'b00;
          crc32_we = 1'b1;
          test_ready_out = 1'b1;
          next_state[2:0] = 3'd0;
        end
      default:
        begin
          sel[1:0] = 2'bxx;
          crc32_we = 1'b0;
          test_ready_out = 1'bx;
          next_state[2:0] = 3'd0;
        end
    endcase
  end

  //--------------------------------------------

  wire word_done;
  reg [11:0] count;

  assign word_done = (state[2:0] == 3'd4);

  always @(posedge clk) begin
    if (rst) begin
      count[11:0] <= 12'h0;
    end else begin
      if (word_done) begin
        count[11:0] <= count[11:0] + 12'h1;
      end
    end
  end

  assign test_step =
    word_done &
    ((count[11:0] == 12'h1FF) | (count[11:0] == 12'h3FF));
  assign test_good =
    ((count[11:0] == 12'h1FF) & (crc32_new[31:0] == 32'h62D5B636)) |
    ((count[11:0] == 12'h3FF) & (crc32_new[31:0] == 32'h866862DF));
  assign test_ended = (count[11:0] == 12'h400);

endmodule
