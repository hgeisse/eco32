//
// cachetest.v -- test the cache
//


`timescale 1ns/10ps
`default_nettype none


//
// define the time to pass before the first request takes place
// (in clock cycles, up to 255)
//
`define HOLDOFF		8'd80

//
// define the time to pass between two consecutive requests
// (in clock cycles, range: 0..15)
//
`define DISTANCE	4'd6
//`define DISTANCE	4'd1
//`define DISTANCE	4'd0


module cachetest(clk, rst,
                 ready_in, valid_out);
    input clk;
    input rst;
    //----------------
    input ready_in;
    output reg valid_out;

  wire holdoff_counting;
  reg [7:0] holdoff;
  wire distance_restart;
  reg [3:0] distance;

  wire gen_step;
  reg [4:0] gen_state;
  reg [4:0] nxt_state;

  wire possibly_valid;

  //--------------------------------------------

  assign holdoff_counting = (holdoff[7:0] != 8'd0) ? 1'b1 : 1'b0;

  always @(posedge clk) begin
    if (rst) begin
      holdoff[7:0] <= `HOLDOFF;
    end else begin
      if (holdoff_counting) begin
        holdoff[7:0] <= holdoff[7:0] - 8'd1;
      end
    end
  end

  assign distance_restart = (distance[3:0] == `DISTANCE) ? 1'b1 : 1'b0;

  always @(posedge clk) begin
    if (holdoff_counting) begin
      distance[3:0] <= 4'd0;
    end else begin
      if (ready_in) begin
        if (distance_restart) begin
          distance[3:0] <= 4'd0;
        end else begin
          distance[3:0] <= distance[3:0] + 4'd1;
        end
      end
    end
  end

  //--------------------------------------------

  assign gen_step = ready_in & distance_restart & ~holdoff_counting;

  always @(posedge clk) begin
    if (rst) begin
      gen_state[4:0] <= 5'h0;
    end else begin
      if (gen_step) begin
        gen_state[4:0] <= nxt_state[4:0];
      end
    end
  end

  assign possibly_valid = (distance[3:0] == 4'd0) ? 1'b1 : 1'b0;

  always @(*) begin
    case (gen_state[4:0])
      5'd0:
        begin
          valid_out = 1'b0;
          nxt_state[4:0] = 5'd1;
        end
      5'd1:
        begin
          valid_out = possibly_valid;
          nxt_state[4:0] = 5'd2;
        end
      5'd2:
        begin
          valid_out = possibly_valid;
          nxt_state[4:0] = 5'd3;
        end
      5'd3:
        begin
          valid_out = possibly_valid;
          nxt_state[4:0] = 5'd1;
        end
      default:
        begin
          valid_out = 1'b0;
          nxt_state[4:0] = 5'd0;
        end
    endcase
  end

  //--------------------------------------------

endmodule
