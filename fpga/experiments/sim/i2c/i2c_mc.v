//
// i2c_mc.v -- I2C master controller
//


`timescale 1ns/10ps
`default_nettype none


`define DEVICE_ADDR	8'h34	// device address (including R/W bit)

`define TIMING_CONST	6'd50	// clock_rate / (3 * bit_rate)

`define BYTE_FNC_START	2'b00	// send START symbol
`define BYTE_FNC_STOP	2'b01	// send STOP symbol
`define BYTE_FNC_SEND	2'b10	// send byte

// NOTE: Do not renumber, coding is exploited!
`define BIT_FNC_START	2'b00	// send START symbol
`define BIT_FNC_STOP	2'b01	// send STOP symbol
`define BIT_FNC_SEND_0	2'b10	// send "0" bit
`define BIT_FNC_SEND_1	2'b11	// send "1" bit, receive a bit


//--------------------------------------------------------------

//
// I2C master controller, write-only
//


module i2c_mc(clk, rst,
              data, start,
              done, error,
              i2c_scl, i2c_sda);
    input clk;
    input rst;
    input [15:0] data;
    input start;
    output reg done;
    output reg error;
    inout i2c_scl;
    inout i2c_sda;

  localparam STATE_IDLE = 4'd0;
  localparam STATE_START_1 = 4'd1;
  localparam STATE_START_2 = 4'd2;
  localparam STATE_ADDR_1 = 4'd3;
  localparam STATE_ADDR_2 = 4'd4;
  localparam STATE_HIBYTE_1 = 4'd5;
  localparam STATE_HIBYTE_2 = 4'd6;
  localparam STATE_LOBYTE_1 = 4'd7;
  localparam STATE_LOBYTE_2 = 4'd8;
  localparam STATE_SUCC_1 = 4'd9;
  localparam STATE_SUCC_2 = 4'd10;
  localparam STATE_FAIL_1 = 4'd11;
  localparam STATE_FAIL_2 = 4'd12;

  reg [3:0] state;
  reg [3:0] next_state;

  reg [1:0] byte_fnc;
  reg [7:0] byte_data;
  reg byte_start;
  wire byte_done;
  wire byte_error;

  always @(posedge clk) begin
    if (rst) begin
      state[3:0] <= STATE_IDLE;
    end else begin
      state[3:0] <= next_state[3:0];
    end
  end

  always @(*) begin
    case (state[3:0])
      STATE_IDLE:
        begin
          if (~start) begin
            next_state[3:0] = STATE_IDLE;
          end else begin
            next_state[3:0] = STATE_START_1;
          end
          byte_fnc[1:0] = 2'bxx;
          byte_data[7:0] = 8'hxx;
          byte_start = 1'b0;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_START_1:
        begin
          next_state[3:0] = STATE_START_2;
          byte_fnc[1:0] = `BYTE_FNC_START;
          byte_data[7:0] = 8'hxx;
          byte_start = 1'b1;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_START_2:
        begin
          if (~byte_done) begin
            next_state[3:0] = STATE_START_2;
          end else begin
            // byte error not possible
            next_state[3:0] = STATE_ADDR_1;
          end
          byte_fnc[1:0] = `BYTE_FNC_START;
          byte_data[7:0] = 8'hxx;
          byte_start = 1'b0;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_ADDR_1:
        begin
          next_state[3:0] = STATE_ADDR_2;
          byte_fnc[1:0] = `BYTE_FNC_SEND;
          byte_data[7:0] = `DEVICE_ADDR;
          byte_start = 1'b1;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_ADDR_2:
        begin
          if (~byte_done) begin
            next_state[3:0] = STATE_ADDR_2;
          end else begin
            if (~byte_error) begin
              next_state[3:0] = STATE_HIBYTE_1;
            end else begin
              next_state[3:0] = STATE_FAIL_1;
            end
          end
          byte_fnc[1:0] = `BYTE_FNC_SEND;
          byte_data[7:0] = `DEVICE_ADDR;
          byte_start = 1'b0;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_HIBYTE_1:
        begin
          next_state[3:0] = STATE_HIBYTE_2;
          byte_fnc[1:0] = `BYTE_FNC_SEND;
          byte_data[7:0] = data[15:8];
          byte_start = 1'b1;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_HIBYTE_2:
        begin
          if (~byte_done) begin
            next_state[3:0] = STATE_HIBYTE_2;
          end else begin
            if (~byte_error) begin
              next_state[3:0] = STATE_LOBYTE_1;
            end else begin
              next_state[3:0] = STATE_FAIL_1;
            end
          end
          byte_fnc[1:0] = `BYTE_FNC_SEND;
          byte_data[7:0] = data[15:8];
          byte_start = 1'b0;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_LOBYTE_1:
        begin
          next_state[3:0] = STATE_LOBYTE_2;
          byte_fnc[1:0] = `BYTE_FNC_SEND;
          byte_data[7:0] = data[7:0];
          byte_start = 1'b1;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_LOBYTE_2:
        begin
          if (~byte_done) begin
            next_state[3:0] = STATE_LOBYTE_2;
          end else begin
            if (~byte_error) begin
              next_state[3:0] = STATE_SUCC_1;
            end else begin
              next_state[3:0] = STATE_FAIL_1;
            end
          end
          byte_fnc[1:0] = `BYTE_FNC_SEND;
          byte_data[7:0] = data[7:0];
          byte_start = 1'b0;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_SUCC_1:
        begin
          next_state[3:0] = STATE_SUCC_2;
          byte_fnc[1:0] = `BYTE_FNC_STOP;
          byte_data[7:0] = 8'hxx;
          byte_start = 1'b1;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_SUCC_2:
        begin
          if (~byte_done) begin
            next_state[3:0] = STATE_SUCC_2;
          end else begin
            // byte error not possible
            next_state[3:0] = STATE_IDLE;
          end
          byte_fnc[1:0] = `BYTE_FNC_STOP;
          byte_data[7:0] = 8'hxx;
          byte_start = 1'b0;
          done = byte_done;
          error = 1'b0;
        end
      STATE_FAIL_1:
        begin
          next_state[3:0] = STATE_FAIL_2;
          byte_fnc[1:0] = `BYTE_FNC_STOP;
          byte_data[7:0] = 8'hxx;
          byte_start = 1'b1;
          done = 1'b0;
          error = 1'b0;
        end
      STATE_FAIL_2:
        begin
          if (~byte_done) begin
            next_state[3:0] = STATE_FAIL_2;
          end else begin
            // byte error not possible
            next_state[3:0] = STATE_IDLE;
          end
          byte_fnc[1:0] = `BYTE_FNC_STOP;
          byte_data[7:0] = 8'hxx;
          byte_start = 1'b0;
          done = byte_done;
          error = byte_done;
        end
      default:
        begin
          next_state[3:0] = STATE_IDLE;
          byte_fnc[1:0] = 2'bxx;
          byte_data[7:0] = 8'hxx;
          byte_start = 1'b0;
          done = 1'b0;
          error = 1'b0;
        end
    endcase
  end

  i2c_byte i2c_byte_0(
    .clk(clk),
    .rst(rst),
    .byte_fnc(byte_fnc[1:0]),
    .byte_data(byte_data[7:0]),
    .byte_start(byte_start),
    .byte_done(byte_done),
    .byte_error(byte_error),
    .i2c_scl(i2c_scl),
    .i2c_sda(i2c_sda)
  );

endmodule


//--------------------------------------------------------------

//
// I2C byte engine
//


module i2c_byte(clk, rst,
                byte_fnc, byte_data, byte_start,
                byte_done, byte_error,
                i2c_scl, i2c_sda);
    input clk;
    input rst;
    input [1:0] byte_fnc;
    input [7:0] byte_data;
    input byte_start;
    output reg byte_done;
    output reg byte_error;
    inout i2c_scl;
    inout i2c_sda;

  localparam STATE_IDLE = 3'd0;
  localparam STATE_START = 3'd1;
  localparam STATE_STOP = 3'd2;
  localparam STATE_SEND_1 = 3'd3;
  localparam STATE_SEND_2 = 3'd4;
  localparam STATE_ACK_1 = 3'd5;
  localparam STATE_ACK_2 = 3'd6;

  reg [2:0] bitcnt;
  reg bitcnt_start;
  reg bitcnt_count;
  wire bitcnt_done;

  always @(posedge clk) begin
    if (rst) begin
      bitcnt[2:0] <= 3'd0;
    end else begin
      if (bitcnt_start) begin
        bitcnt[2:0] <= 3'h7;
      end else begin
        if (~bitcnt_done) begin
          if (bitcnt_count) begin
            bitcnt[2:0] <= bitcnt[2:0] - 3'd1;
          end
        end
      end
    end
  end

  assign bitcnt_done = (bitcnt[2:0] == 3'd0);

  reg [2:0] state;
  reg [2:0] next_state;

  reg [1:0] bit_fnc;
  reg bit_start;
  wire bit_done;
  wire bit_rcv;

  always @(posedge clk) begin
    if (rst) begin
      state[2:0] <= STATE_IDLE;
    end else begin
      state[2:0] <= next_state[2:0];
    end
  end

  always @(*) begin
    case (state[2:0])
      STATE_IDLE:
        begin
          if (~byte_start) begin
            next_state[2:0] = STATE_IDLE;
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b0;
            bit_fnc[1:0] = 2'bxx;
            bit_start = 1'b0;
            byte_done = 1'b0;
            byte_error = 1'b0;
          end else begin
            case (byte_fnc[1:0])
              `BYTE_FNC_START:
                begin
                  next_state[2:0] = STATE_START;
                  bitcnt_start = 1'b0;
                  bitcnt_count = 1'b0;
                  bit_fnc[1:0] = `BIT_FNC_START;
                  bit_start = 1'b1;
                  byte_done = 1'b0;
                  byte_error = 1'b0;
                end
              `BYTE_FNC_STOP:
                begin
                  next_state[2:0] = STATE_STOP;
                  bitcnt_start = 1'b0;
                  bitcnt_count = 1'b0;
                  bit_fnc[1:0] = `BIT_FNC_STOP;
                  bit_start = 1'b1;
                  byte_done = 1'b0;
                  byte_error = 1'b0;
                end
              `BYTE_FNC_SEND:
                begin
                  next_state[2:0] = STATE_SEND_1;
                  bitcnt_start = 1'b1;
                  bitcnt_count = 1'b0;
                  bit_fnc[1:0] = { 1'b1, byte_data[7] };
                  bit_start = 1'b0;
                  byte_done = 1'b0;
                  byte_error = 1'b0;
                end
              default:
                begin
                  next_state[2:0] = STATE_IDLE;
                  bitcnt_start = 1'b0;
                  bitcnt_count = 1'b0;
                  bit_fnc[1:0] = 2'bxx;
                  bit_start = 1'b0;
                  byte_done = 1'b0;
                  byte_error = 1'b0;
                end
            endcase
          end
        end
      STATE_START:
        begin
          if (~bit_done) begin
            next_state[2:0] = STATE_START;
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b0;
            bit_fnc[1:0] = `BIT_FNC_START;
            bit_start = 1'b0;
            byte_done = 1'b0;
            byte_error = 1'b0;
          end else begin
            next_state[2:0] = STATE_IDLE;
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b0;
            bit_fnc[1:0] = `BIT_FNC_START;
            bit_start = 1'b0;
            byte_done = 1'b1;
            byte_error = 1'b0;
          end
        end
      STATE_STOP:
        begin
          if (~bit_done) begin
            next_state[2:0] = STATE_STOP;
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b0;
            bit_fnc[1:0] = `BIT_FNC_STOP;
            bit_start = 1'b0;
            byte_done = 1'b0;
            byte_error = 1'b0;
          end else begin
            next_state[2:0] = STATE_IDLE;
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b0;
            bit_fnc[1:0] = `BIT_FNC_STOP;
            bit_start = 1'b0;
            byte_done = 1'b1;
            byte_error = 1'b0;
          end
        end
      STATE_SEND_1:
        begin
          next_state[2:0] = STATE_SEND_2;
          bitcnt_start = 1'b0;
          bitcnt_count = 1'b0;
          bit_fnc[1:0] = { 1'b1, byte_data[bitcnt] };
          bit_start = 1'b1;
          byte_done = 1'b0;
          byte_error = 1'b0;
        end
      STATE_SEND_2:
        begin
          if (~bit_done) begin
            next_state[2:0] = STATE_SEND_2;
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b0;
            bit_fnc[1:0] = { 1'b1, byte_data[bitcnt] };
            bit_start = 1'b0;
            byte_done = 1'b0;
            byte_error = 1'b0;
          end else begin
            if (~bitcnt_done) begin
              next_state[2:0] = STATE_SEND_1;
            end else begin
              next_state[2:0] = STATE_ACK_1;
            end
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b1;
            bit_fnc[1:0] = { 1'b1, byte_data[bitcnt] };
            bit_start = 1'b0;
            byte_done = 1'b0;
            byte_error = 1'b0;
          end
        end
      STATE_ACK_1:
        begin
          next_state[2:0] = STATE_ACK_2;
          bitcnt_start = 1'b0;
          bitcnt_count = 1'b0;
          bit_fnc[1:0] = { 1'b1, 1'b1 };
          bit_start = 1'b1;
          byte_done = 1'b0;
          byte_error = 1'b0;
        end
      STATE_ACK_2:
        begin
          if (~bit_done) begin
            next_state[2:0] = STATE_ACK_2;
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b0;
            bit_fnc[1:0] = { 1'b1, 1'b1 };
            bit_start = 1'b0;
            byte_done = 1'b0;
            byte_error = 1'b0;
          end else begin
            next_state[2:0] = STATE_IDLE;
            bitcnt_start = 1'b0;
            bitcnt_count = 1'b0;
            bit_fnc[1:0] = { 1'b1, 1'b1 };
            bit_start = 1'b0;
            byte_done = 1'b1;
            byte_error = bit_rcv;
          end
        end
      default:
        begin
          next_state[2:0] = STATE_IDLE;
          bitcnt_start = 1'b0;
          bitcnt_count = 1'b0;
          bit_fnc[1:0] = 2'bxx;
          bit_start = 1'b0;
          byte_done = 1'b0;
          byte_error = 1'b0;
        end
    endcase
  end

  i2c_bit i2c_bit_0(
    .clk(clk),
    .rst(rst),
    .bit_fnc(bit_fnc[1:0]),
    .bit_start(bit_start),
    .bit_done(bit_done),
    .bit_rcv(bit_rcv),
    .i2c_scl(i2c_scl),
    .i2c_sda(i2c_sda)
  );

endmodule


//--------------------------------------------------------------

//
// I2C bit engine
//


module i2c_bit(clk, rst,
               bit_fnc, bit_start,
               bit_done, bit_rcv,
               i2c_scl, i2c_sda);
    input clk;
    input rst;
    input [1:0] bit_fnc;
    input bit_start;
    output reg bit_done;
    output reg bit_rcv;
    inout i2c_scl;
    inout i2c_sda;

  localparam STATE_IDLE = 4'd0;
  localparam STATE_START_A = 4'd1;
  localparam STATE_START_B = 4'd2;
  localparam STATE_START_C = 4'd3;
  localparam STATE_STOP_A = 4'd4;
  localparam STATE_STOP_B = 4'd5;
  localparam STATE_STOP_C = 4'd6;
  localparam STATE_SEND_A = 4'd7;
  localparam STATE_SEND_B = 4'd8;
  localparam STATE_SEND_C = 4'd9;

  reg [5:0] clkcnt;
  reg clkcnt_start;
  wire clkcnt_done;
  wire clkcnt_center;

  always @(posedge clk) begin
    if (rst) begin
      clkcnt[5:0] <= 6'd0;
    end else begin
      if (clkcnt_start) begin
        clkcnt[5:0] <= `TIMING_CONST;
      end else begin
        if (~clkcnt_done) begin
          clkcnt[5:0] <= clkcnt[5:0] - 6'd1;
        end
      end
    end
  end

  assign clkcnt_done = (clkcnt[5:0] == 6'd0);
  assign clkcnt_center = (clkcnt[5:0] == `TIMING_CONST / 2 - 2);

  reg [3:0] state;
  reg [3:0] next_state;

  always @(posedge clk) begin
    if (rst) begin
      state[3:0] <= STATE_IDLE;
    end else begin
      state[3:0] <= next_state[3:0];
    end
  end

  always @(*) begin
    case (state[3:0])
      STATE_IDLE:
        begin
          if (~bit_start) begin
            next_state[3:0] = STATE_IDLE;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            case (bit_fnc[1:0])
              `BIT_FNC_START:
                // SCL = 110
                // SDA = 100
                begin
                  next_state[3:0] = STATE_START_A;
                  clkcnt_start = 1'b1;
                  bit_done = 1'b0;
                end
              `BIT_FNC_STOP:
                // SCL = 011
                // SDA = 001
                begin
                  next_state[3:0] = STATE_STOP_A;
                  clkcnt_start = 1'b1;
                  bit_done = 1'b0;
                end
              `BIT_FNC_SEND_0,
              `BIT_FNC_SEND_1:
                // SCL = 010
                // SDA = bbb
                begin
                  next_state[3:0] = STATE_SEND_A;
                  clkcnt_start = 1'b1;
                  bit_done = 1'b0;
                end
              default:
                begin
                  next_state[3:0] = STATE_IDLE;
                  clkcnt_start = 1'b0;
                  bit_done = 1'b0;
                end
            endcase
          end
        end
      STATE_START_A:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_START_A;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_START_B;
            clkcnt_start = 1'b1;
            bit_done = 1'b0;
          end
        end
      STATE_START_B:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_START_B;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_START_C;
            clkcnt_start = 1'b1;
            bit_done = 1'b0;
          end
        end
      STATE_START_C:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_START_C;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_IDLE;
            clkcnt_start = 1'b0;
            bit_done = 1'b1;
          end
        end
      STATE_STOP_A:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_STOP_A;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_STOP_B;
            clkcnt_start = 1'b1;
            bit_done = 1'b0;
          end
        end
      STATE_STOP_B:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_STOP_B;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_STOP_C;
            clkcnt_start = 1'b1;
            bit_done = 1'b0;
          end
        end
      STATE_STOP_C:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_STOP_C;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_IDLE;
            clkcnt_start = 1'b0;
            bit_done = 1'b1;
          end
        end
      STATE_SEND_A:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_SEND_A;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_SEND_B;
            clkcnt_start = 1'b1;
            bit_done = 1'b0;
          end
        end
      STATE_SEND_B:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_SEND_B;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_SEND_C;
            clkcnt_start = 1'b1;
            bit_done = 1'b0;
          end
        end
      STATE_SEND_C:
        begin
          if (~clkcnt_done) begin
            next_state[3:0] = STATE_SEND_C;
            clkcnt_start = 1'b0;
            bit_done = 1'b0;
          end else begin
            next_state[3:0] = STATE_IDLE;
            clkcnt_start = 1'b0;
            bit_done = 1'b1;
          end
        end
      default:
        begin
          next_state[3:0] = STATE_IDLE;
          clkcnt_start = 1'b0;
          bit_done = 1'b0;
        end
    endcase
  end

  reg scl_snd;
  reg sda_snd;

  always @(posedge clk) begin
    if (rst) begin
      scl_snd <= 1'b1;
      sda_snd <= 1'b1;
    end else begin
      if (state[3:0] == STATE_START_A) begin
        scl_snd <= 1'b1;
        sda_snd <= 1'b1;
      end
      if (state[3:0] == STATE_START_B) begin
        scl_snd <= 1'b1;
        sda_snd <= 1'b0;
      end
      if (state[3:0] == STATE_START_C) begin
        scl_snd <= 1'b0;
        sda_snd <= 1'b0;
      end
      if (state[3:0] == STATE_STOP_A) begin
        scl_snd <= 1'b0;
        sda_snd <= 1'b0;
      end
      if (state[3:0] == STATE_STOP_B) begin
        scl_snd <= 1'b1;
        sda_snd <= 1'b0;
      end
      if (state[3:0] == STATE_STOP_C) begin
        scl_snd <= 1'b1;
        sda_snd <= 1'b1;
      end
      if (state[3:0] == STATE_SEND_A) begin
        scl_snd <= 1'b0;
        sda_snd <= bit_fnc[0];
      end
      if (state[3:0] == STATE_SEND_B) begin
        scl_snd <= 1'b1;
        sda_snd <= bit_fnc[0];
      end
      if (state[3:0] == STATE_SEND_C) begin
        scl_snd <= 1'b0;
        sda_snd <= bit_fnc[0];
      end
    end
  end

  wire sda_rcv;

  always @(posedge clk) begin
    if (rst) begin
      bit_rcv <= 1'b0;
    end else begin
      if ((state == STATE_SEND_B) & clkcnt_center) begin
        bit_rcv <= sda_rcv;
      end
    end
  end

  i2c_phy i2c_phy_0(
    .clk(clk),
    .scl_snd(scl_snd),
    .sda_snd(sda_snd),
    .sda_rcv(sda_rcv),
    .i2c_scl(i2c_scl),
    .i2c_sda(i2c_sda)
  );

endmodule


//--------------------------------------------------------------

//
// I2C physical layer
//


module i2c_phy(clk,
               scl_snd, sda_snd,
               sda_rcv,
               i2c_scl, i2c_sda);
    input clk;
    input scl_snd;
    input sda_snd;
    output reg sda_rcv;
    inout i2c_scl;
    inout i2c_sda;

  reg sda_pre;

  assign i2c_scl = scl_snd ? 1'bz : 1'b0;
  assign i2c_sda = sda_snd ? 1'bz : 1'b0;

  always @(posedge clk) begin
    sda_pre <= i2c_sda;
    sda_rcv <= sda_pre;
  end

endmodule
