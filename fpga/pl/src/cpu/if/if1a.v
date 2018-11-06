//
// if1a.v -- instruction fetch, stage 1a (read counter)
//


`timescale 1ns/10ps
`default_nettype none


//
// define the time to pass before the first request takes place
// (in clock cycles, up to 255)
//
`define HOLDOFF		8'd20

//
// define the time to pass between two consecutive requests
// (in clock cycles, range: 0..15)
//
`define DISTANCE	4'd6
//`define DISTANCE	4'd1
//`define DISTANCE	4'd0


module if1a(clk, rst,
            if1a_ready_in,
            if1a_valid_out,
            if1a_counter_out);
    input clk;
    input rst;
    //----------------
    input if1a_ready_in;
    output if1a_valid_out;
    output [9:0] if1a_counter_out;

  reg [7:0] holdoff;
  wire holdoff_counting;
  reg [3:0] distance;
  wire distance_restart;

  reg valid;
  reg [9:0] counter;

  //--------------------------------------------

  always @(posedge clk) begin
    if (rst) begin
      holdoff[7:0] <= `HOLDOFF;
    end else begin
      if (holdoff_counting) begin
        holdoff[7:0] <= holdoff[7:0] - 8'd1;
      end
    end
  end

  assign holdoff_counting = (holdoff[7:0] != 8'd0) ? 1'b1 : 1'b0;

  always @(posedge clk) begin
    if (holdoff_counting) begin
      distance[3:0] <= 4'd0;
    end else begin
      if (if1a_ready_in) begin
        if (distance_restart) begin
          distance[3:0] <= 4'd0;
        end else begin
          distance[3:0] <= distance[3:0] + 4'd1;
        end
      end
    end
  end

  assign distance_restart = (distance[3:0] == `DISTANCE) ? 1'b1 : 1'b0;

  //--------------------------------------------

  always @(posedge clk) begin
    if (rst) begin
      valid <= 1'b0;
    end else begin
      if (if1a_ready_in) begin
        valid <= distance_restart & ~holdoff_counting;
      end
    end
  end

  assign if1a_valid_out = valid;

  always @(posedge clk) begin
    if (rst) begin
      counter[9:0] <= ~10'h000;
    end else begin
      if (if1a_ready_in & distance_restart & ~holdoff_counting) begin
        counter[9:0] <= counter[9:0] + 10'h001;
      end
    end
  end

  assign if1a_counter_out[9:0] = valid ? counter[9:0] : 10'hxxx;

endmodule
