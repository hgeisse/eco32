//
// pipe.v -- a very simple 5-stage pipeline
//


`timescale 1ns/10ps
`default_nettype none


module pipe(clk, rst,
            test_ended,
            test_error);
    input clk;
    input rst;
    output reg test_ended;
    output reg test_error;

  wire st0_ready;
  reg st0_valid_in;
  reg [7:0] st0_data_in;
  wire st0_stall;
  wire st0_valid_out;
  wire [7:0] st0_data_out;

  wire st1_ready;
  reg st1_valid_in;
  reg [7:0] st1_data_in;
  wire st1_stall;
  wire st1_valid_out;
  wire [7:0] st1_data_out;

  wire st2_ready;
  reg st2_valid_in;
  reg [7:0] st2_data_in;
  wire st2_stall;
  wire st2_valid_out;
  wire [7:0] st2_data_out;

  wire st3_ready;
  reg st3_valid_in;
  reg [7:0] st3_data_in;
  wire st3_stall;
  wire st3_valid_out;
  wire [7:0] st3_data_out;

  wire st4_ready;
  reg st4_valid_in;
  reg [7:0] st4_data_in;
  wire st4_stall;
  wire st4_valid_out;
  wire [7:0] st4_data_out;

  //------------------------------------

  assign st0_ready = st1_ready & ~st0_stall;

  always @(posedge clk) begin
    if (rst) begin
      st0_valid_in <= 1'b0;
      st0_data_in[7:0] <= 8'hFF;
    end else begin
      if (st0_ready) begin
        st0_valid_in <= 1'b1;
        st0_data_in[7:0] <= st0_data_in[7:0] + 8'h01;
      end
    end
  end

  //------------------------------------

  assign st0_stall = 1'b0;
  assign st0_valid_out = st0_valid_in & ~st0_stall;
  assign st0_data_out[7:0] = st0_stall ? 8'hxx : st0_data_in[7:0];

  //------------------------------------

  assign st1_ready = st2_ready & ~st1_stall;

  always @(posedge clk) begin
    if (st1_ready) begin
      st1_valid_in <= st0_valid_out;
      st1_data_in[7:0] <= st0_data_out[7:0];
    end
  end

  //------------------------------------

  reg [3:0] st1_count;

  always @(posedge clk) begin
    if (rst) begin
      st1_count[3:0] <= 4'd0;
    end else begin
      if (st1_count[3:0] == 4'd10) begin
        st1_count[3:0] <= 4'd0;
      end else begin
        st1_count[3:0] <= st1_count[3:0] + 4'd1;
      end
    end
  end

  assign st1_stall =
    (st1_count[3:0] == 4'd1 || st1_count[3:0] == 4'd7);
  assign st1_valid_out = st1_valid_in & ~st1_stall;
  assign st1_data_out[7:0] = st1_stall ? 8'hxx : st1_data_in[7:0];

  //------------------------------------

  assign st2_ready = st3_ready & ~st2_stall;

  always @(posedge clk) begin
    if (st2_ready) begin
      st2_valid_in <= st1_valid_out;
      st2_data_in[7:0] <= st1_data_out[7:0];
    end
  end

  //------------------------------------

  reg [2:0] st2_count;

  always @(posedge clk) begin
    if (rst) begin
      st2_count[2:0] <= 3'd0;
    end else begin
      if (st2_count[2:0] == 3'd6) begin
        st2_count[2:0] <= 3'd0;
      end else begin
        st2_count[2:0] <= st2_count[2:0] + 3'd1;
      end
    end
  end

  assign st2_stall =
    (st2_count[2:0] == 3'd4 || st2_count[2:0] == 3'd5);
  assign st2_valid_out = st2_valid_in & ~st2_stall;
  assign st2_data_out[7:0] = st2_stall ? 8'hxx : st2_data_in[7:0];

  //------------------------------------

  assign st3_ready = st4_ready & ~st3_stall;

  always @(posedge clk) begin
    if (st3_ready) begin
      st3_valid_in <= st2_valid_out;
      st3_data_in[7:0] <= st2_data_out[7:0];
    end
  end

  //------------------------------------

  assign st3_stall = 1'b0;
  assign st3_valid_out = st3_valid_in & ~st3_stall;
  assign st3_data_out[7:0] = st3_stall ? 8'hxx : st3_data_in[7:0];

  //------------------------------------

  assign st4_ready = ~st4_stall;

  always @(posedge clk) begin
    if (st4_ready) begin
      st4_valid_in <= st3_valid_out;
      st4_data_in[7:0] <= st3_data_out[7:0];
    end
  end

  //------------------------------------

  reg [4:0] st4_count;

  always @(posedge clk) begin
    if (rst) begin
      st4_count[4:0] <= 5'd0;
    end else begin
      if (st4_count[4:0] == 5'd27) begin
        st4_count[4:0] <= 5'd0;
      end else begin
        st4_count[4:0] <= st4_count[4:0] + 5'd1;
      end
    end
  end

  assign st4_stall =
    (st4_count[4:0] == 5'd19 || st4_count[4:0] == 5'd20 ||
     st4_count[4:0] == 5'd21 || st4_count[4:0] == 5'd22 ||
     st4_count[4:0] == 5'd23);
  assign st4_valid_out = st4_valid_in & ~st4_stall;
  assign st4_data_out[7:0] = st4_stall ? 8'hxx : st4_data_in[7:0];

  //------------------------------------

  reg [9:0] state;

  always @(posedge clk) begin
    if (rst) begin
      state[9:0] <= 10'h000;
      test_ended <= 1'b0;
      test_error <= 1'b0;
    end else begin
      if (state[9:0] == 10'h220) begin
        if (st4_valid_out & (st4_data_out[7:0] == 8'hFE)) begin
          state[9:0] <= 10'h221;
        end else begin
          state[9:0] <= 10'h227;
        end
      end else
      if (state[9:0] == 10'h223) begin
        if (st4_valid_out & (st4_data_out[7:0] == 8'hFF)) begin
          state[9:0] <= 10'h224;
        end else begin
          state[9:0] <= 10'h227;
        end
      end else
      if (state[9:0] == 10'h224) begin
        if (st4_valid_out & (st4_data_out[7:0] == 8'h00)) begin
          state[9:0] <= 10'h225;
        end else begin
          state[9:0] <= 10'h227;
        end
      end else
      if (state[9:0] == 10'h226) begin
        if (st4_valid_out & (st4_data_out[7:0] == 8'h01)) begin
          state[9:0] <= 10'h228;
        end else begin
          state[9:0] <= 10'h227;
        end
      end else
      if (state[9:0] == 10'h227) begin
        // failure
        state[9:0] <= 10'h227;
        test_ended <= 1'b1;
        test_error <= 1'b1;
      end else
      if (state[9:0] == 10'h228) begin
        // success
        state[9:0] <= 10'h228;
        test_ended <= 1'b1;
        test_error <= 1'b0;
      end else begin
        state[9:0] <= state[9:0] + 10'h001;
      end
    end
  end

endmodule
