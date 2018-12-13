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

//
// define the total number of requests
//
`define NUM_TESTS	12'h100

//
// operation to be performed at end of pipeline
//
`define OP_NOP		2'b00
`define OP_CLR		2'b01
`define OP_SUM		2'b10
`define OP_CMP		2'b11


module cachetest(clk, rst,
                 ready_in, valid_out,
                 rd_out, wr_out, addr_out, data_out,
                 ready_out, valid_in, data_in,
                 test_ended, test_error);
    input clk;
    input rst;
    //----------------
    input ready_in;
    output valid_out;
    output rd_out;
    output wr_out;
    output [15:0] addr_out;
    output [7:0] data_out;
    //----------------
    output ready_out;
    input valid_in;
    input [7:0] data_in;
    //----------------
    output reg test_ended;
    output reg test_error;

  wire holdoff_counting;
  reg [7:0] holdoff;
  wire distance_restart;
  reg [3:0] distance;

  wire gen_step;
  reg [4:0] gen_state;
  reg [4:0] nxt_state;

  reg valid;
  reg read;
  reg write;
  reg addr_load;
  reg [7:0] addr_cnst;
  reg addr_incr;
  reg addr_skip;
  reg [7:0] addr_offs;
  reg data_sel;
  reg [1:0] op;

  reg [7:0] addr;
  wire [7:0] addr2;
  reg [15:0] addr3;
  wire [7:0] data;

  wire data_error;
  reg [11:0] test_count;

  //--------------------------------------------

  assign holdoff_counting = (holdoff[7:0] != 8'd0);

  always @(posedge clk) begin
    if (rst) begin
      holdoff[7:0] <= `HOLDOFF;
    end else begin
      if (holdoff_counting) begin
        holdoff[7:0] <= holdoff[7:0] - 8'd1;
      end
    end
  end

  assign distance_restart = (distance[3:0] == `DISTANCE);

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

  assign gen_step = ready_in & distance_restart & ~holdoff_counting;

  always @(posedge clk) begin
    if (rst) begin
      gen_state[4:0] <= 5'h00;
    end else begin
      if (gen_step) begin
        gen_state[4:0] <= nxt_state[4:0];
      end
    end
  end

  always @(*) begin
    case (gen_state[4:0])
      //
      // initialize matrix
      //
      5'h00:
        // reset address counter to m[0][0]
        begin
          valid = 1'b0;
          read = 1'b0;
          write = 1'b0;
          addr_load = 1'b1;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_NOP;
          nxt_state[4:0] = 5'h01;
        end
      5'h01:
        // write matrix elements (data = own address)
        begin
          valid = 1'b1;
          read = 1'b0;
          write = 1'b1;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b1;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_NOP;
          if (addr[7:0] == 8'hFF) begin
            nxt_state[4:0] = 5'h02;
          end else begin
            nxt_state[4:0] = 5'h01;
          end
        end
      //
      // compute new matrix
      //
      5'h02:
        // reset address counter to m[1][1]
        // clear sum register (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b0;
          write = 1'b0;
          addr_load = 1'b1;
          addr_cnst[7:0] = 8'h11;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_CLR;
          nxt_state[4:0] = 5'h03;
        end
      5'h03:
        // add m[i-1][j-1] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'hEF;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h04;
        end
      5'h04:
        // add m[i-1][j] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'hF0;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h05;
        end
      5'h05:
        // add m[i-1][j+1] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'hF1;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h06;
        end
      5'h06:
        // add m[i][j-1] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'hFF;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h07;
        end
      5'h07:
        // add m[i][j] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h08;
        end
      5'h08:
        // add m[i][j+1] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h01;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h09;
        end
      5'h09:
        // add m[i+1][j-1] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h0F;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h0A;
        end
      5'h0A:
        // add m[i+1][j] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h10;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h0B;
        end
      5'h0B:
        // add m[i+1][j+1] to sum (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h11;
          data_sel = 1'b0;
          op[1:0] = `OP_SUM;
          nxt_state[4:0] = 5'h0C;
        end
      5'h0C:
        // wait one clock cycle for sum to stabilize
        begin
          valid = 1'b0;
          read = 1'b0;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_NOP;
          nxt_state[4:0] = 5'h0D;
        end
      5'h0D:
        // write new matrix element m[i][j]
        // clear sum register (other end of pipeline)
        begin
          valid = 1'b1;
          read = 1'b0;
          write = 1'b1;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b1;
          addr_skip = 1'b1;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b1;
          op[1:0] = `OP_CLR;
          if (addr[7:0] == 8'hEE) begin
            nxt_state[4:0] = 5'h0E;
          end else begin
            nxt_state[4:0] = 5'h03;
          end
        end
      //
      // read new matrix
      //
      5'h0E:
        // reset address counter to m[0][0]
        begin
          valid = 1'b0;
          read = 1'b0;
          write = 1'b0;
          addr_load = 1'b1;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_NOP;
          nxt_state[4:0] = 5'h0F;
        end
      5'h0F:
        // read matrix elements
        begin
          valid = 1'b1;
          read = 1'b1;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b1;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_CMP;
          if (addr[7:0] == 8'hFF) begin
            nxt_state[4:0] = 5'h10;
          end else begin
            nxt_state[4:0] = 5'h0F;
          end
        end
      //
      // halt
      //
      5'h10:
        // get stuck in this state
        begin
          valid = 1'b0;
          read = 1'b0;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_NOP;
          nxt_state[4:0] = 5'h10;
        end
      //
      // not used
      //
      default:
        begin
          valid = 1'b0;
          read = 1'b0;
          write = 1'b0;
          addr_load = 1'b0;
          addr_cnst[7:0] = 8'h00;
          addr_incr = 1'b0;
          addr_skip = 1'b0;
          addr_offs[7:0] = 8'h00;
          data_sel = 1'b0;
          op[1:0] = `OP_NOP;
          nxt_state[4:0] = 5'h00;
        end
    endcase
  end

  always @(posedge clk) begin
    if (gen_step) begin
      if (addr_load) begin
        addr[7:0] <= addr_cnst[7:0];
      end else begin
        if (addr_incr) begin
          if (addr_skip & (addr[3:0] == 4'hE)) begin
            addr[7:0] <= addr[7:0] + 8'h03;
          end else begin
            addr[7:0] <= addr[7:0] + 8'h01;
          end
        end
      end
    end
  end

  assign addr2[7:0] = addr[7:0] + addr_offs[7:0];

  always @(*) begin
    case (addr2[7:5])
      3'b000: addr3[15:0] = { 8'h11, addr2[7:0] };
      3'b001: addr3[15:0] = { 8'h21, addr2[7:0] };
      3'b010: addr3[15:0] = { 8'h31, addr2[7:0] };
      3'b011: addr3[15:0] = { 8'h41, addr2[7:0] };
      3'b100: addr3[15:0] = { 8'h51, addr2[7:0] };
      3'b101: addr3[15:0] = { 8'h61, addr2[7:0] };
      3'b110: addr3[15:0] = { 8'h71, addr2[7:0] };
      3'b111: addr3[15:0] = { 8'h81, addr2[7:0] };
    endcase
  end

  assign valid_out = valid & (distance[3:0] == 4'd0);
  assign rd_out = valid_out ? read : 1'bx;
  assign wr_out = valid_out ? write : 1'bx;
  assign addr_out[15:0] = valid_out ? addr3[15:0] : 16'hxxxx;
  assign data[7:0] = ~data_sel ? addr[7:0] : sum[7:0];
  assign data_out[7:0] = valid_out ? data[7:0] : 8'hxx;

  //--------------------------------------------

  reg [1:0] op_eop;

  always @(posedge clk) begin
    if (ready_in) begin
      op_eop[1:0] <= op[1:0];
    end
  end

  //--------------------------------------------

  reg [7:0] sum;

  always @(posedge clk) begin
    if (valid_in & (op_eop[1:0] == `OP_CLR)) begin
      sum[7:0] <= 8'h00;
    end else begin
      if (valid_in & (op_eop[1:0] == `OP_SUM)) begin
        sum[7:0] <= sum[7:0] ^ data_in[7:0];
      end
    end
  end

  //--------------------------------------------

  reg [7:0] ref[0:255];
  wire [7:0] ref_out;

  initial begin
    #0          $readmemh("ref.dat", ref);
  end

  assign ref_out = ref[test_count[7:0]];

  //--------------------------------------------

  assign ready_out = 1'b1;

  assign data_error = (data_in[7:0] != ref_out[7:0]);

  always @(posedge clk) begin
    if (rst) begin
      test_count[11:0] <= 12'h0;
      test_ended <= 1'b0;
      test_error <= 1'b0;
    end else begin
      if (test_count[11:0] != `NUM_TESTS) begin
        if (valid_in & (op_eop[1:0] == `OP_CMP)) begin
          test_count[11:0] <= test_count[11:0] + 12'h1;
          if (data_error) begin
            test_error <= 1'b1;
          end
        end
      end else begin
        test_ended <= 1'b1;
      end
    end
  end

endmodule
