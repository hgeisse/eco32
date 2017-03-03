//
// memspeed.v -- toplevel for memory speedometer
//


`timescale 1ns/10ps
`default_nettype none


module memspeed(clk_in,
                rst_inout_n,
                sdram_clk,
                sdram_fb,
                sdram_cke,
                sdram_cs_n,
                sdram_ras_n,
                sdram_cas_n,
                sdram_we_n,
                sdram_ba,
                sdram_a,
                sdram_udqm,
                sdram_ldqm,
                sdram_dq,
                ssl
               );
    // clock and reset
    input clk_in;
    inout rst_inout_n;
    // SDRAM
    output sdram_clk;
    input sdram_fb;
    output sdram_cke;
    output sdram_cs_n;
    output sdram_ras_n;
    output sdram_cas_n;
    output sdram_we_n;
    output [1:0] sdram_ba;
    output [12:0] sdram_a;
    output sdram_udqm;
    output sdram_ldqm;
    inout [15:0] sdram_dq;
    // 7 segment LED output
    output [6:0] ssl;

  // clk_rst
  wire clk_ok;
  wire clk;
  wire rst;
  // ram
  reg stb;
  wire we;
  wire [25:0] inst_addr;
  wire [63:0] inst_dout;
  wire inst_ack;
  wire inst_timeout;
  wire [25:0] data_addr;
  wire [63:0] data_din;
  wire [63:0] data_dout;
  wire data_ack;
  wire data_timeout;
  // control
  reg [27:0] count;
  reg next_count;
  reg [1:0] state;
  reg [1:0] next_state;

  clk_rst clk_rst_1(
    .clk_in(clk_in),
    .rst_inout_n(rst_inout_n),
    .sdram_clk(sdram_clk),
    .sdram_fb(sdram_fb),
    .clk_ok(clk_ok),
    .clk(clk),
    .rst(rst)
  );

  ramctrl ramctrl_1(
    .clk_ok(clk_ok),
    .clk(clk),
    .inst_stb(stb & (~count[0] & ~we)),
    .inst_addr(inst_addr[25:0]),
    .inst_dout(inst_dout[63:0]),
    .inst_ack(inst_ack),
    .inst_timeout(inst_timeout),
    .data_stb(stb & (count[0] | we)),
    .data_we(we),
    .data_addr(data_addr[25:0]),
    .data_din(data_din[63:0]),
    .data_dout(data_dout[63:0]),
    .data_ack(data_ack),
    .data_timeout(data_timeout),
    .sdram_cke(sdram_cke),
    .sdram_cs_n(sdram_cs_n),
    .sdram_ras_n(sdram_ras_n),
    .sdram_cas_n(sdram_cas_n),
    .sdram_we_n(sdram_we_n),
    .sdram_ba(sdram_ba),
    .sdram_a(sdram_a),
    .sdram_udqm(sdram_udqm),
    .sdram_ldqm(sdram_ldqm),
    .sdram_dq(sdram_dq)
  );

  assign we = count[1] & count[0];
  assign inst_addr[25:0] = { count[27], count[27],
                             count[27], count[27],
                             count[21:0] ^ 23'h1234 };
  assign data_addr[25:0] = { count[27], count[27],
                             count[27], count[27],
                             count[21:0] ^ 23'h2345 };
  assign data_din[63:0] = { count[15:0], count[15:0],
                            count[15:0], count[15:0] };

  always @(posedge clk) begin
    if (rst) begin
      count <= 0;
    end else begin
      if (next_count) begin
        count <= count + 1;
      end
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      state <= 0;
    end else begin
      state <= next_state;
    end
  end

  always @(*) begin
    case (state)
      2'd0:
        begin
          stb = 0;
          next_count = 0;
          next_state = 1;
        end
      2'd1:
        begin
          stb = 1;
          next_count = 0;
          if (inst_ack | inst_timeout |
              data_ack | data_timeout) begin
            next_state = 2;
          end else begin
            next_state = 1;
          end
        end
      2'd2:
        begin
          stb = 0;
          next_count = 1;
          if (count[27]) begin
            next_state = 3;
          end else begin
            next_state = 1;
          end
        end
      2'd3:
        begin
          stb = 0;
          next_count = 0;
          next_state = 3;
        end
    endcase
  end

  assign ssl[0] = 0;
  assign ssl[1] = | state[1:0];
  assign ssl[2] = & state[1:0];
  assign ssl[3] = 0;
  assign ssl[4] = inst_timeout;
  assign ssl[5] = data_timeout;
  assign ssl[6] = ^ (inst_dout[63:0] | data_dout[63:0]);

endmodule
