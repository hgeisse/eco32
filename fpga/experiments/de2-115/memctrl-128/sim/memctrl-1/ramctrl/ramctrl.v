//
// ramctrl.v -- RAM controller
//


`define den512Mb
`define sg7e
`define x16
`include "ramctrl/sdr.v"


`timescale 1ns/10ps
`default_nettype none


`define MODE		13'h0022	// CL = 2, sequ. burst length = 4

`define CMD_MRSET	3'b000		// mode register set
`define CMD_ARFRS	3'b001		// auto refresh
`define CMD_PRCHG	3'b010		// precharge (deactivate row/rows)
`define CMD_ACTV	3'b011		// select bank, activate row
`define CMD_WRITE	3'b100		// select bank & column, start write
`define CMD_READ	3'b101		// select bank & column, start read
`define CMD_BSTOP	3'b110		// burst stop
`define CMD_NOP		3'b111		// no operation

//
// Note: The FSM is a registered Mealy machine. Its actions, which
//       are noted here for a specific state, take place in the next
//       clock cycle. This is only a notational problem: the actions
//       should in fact be associated with state transitions.
//
// ST_RESET				// NOP, CKE=0, CS_N=1, wait 100 us
`define ST_INIT0	5'd0		// NOP, CKE=1, CS_N=0, wait 100 us
`define ST_INIT1	5'd1		// PRECHARGE ALL
`define ST_INIT2	5'd2		// NOP, wait tRP - 1 cycle
`define ST_INIT3	5'd3		// AUTO REFRESH
`define ST_INIT4	5'd4		// NOP, wait tRFC - 1 cycle
`define ST_INIT5	5'd5		// AUTO REFRESH
`define ST_INIT6	5'd6		// NOP, wait tRFC - 1 cycle
`define ST_INIT7	5'd7		// MODE REGISTER SET
`define ST_INIT8	5'd8		// NOP, wait tMRD - 1 cycle
`define ST_IDLE		5'd9		// AUTO REFRESH, ACTIVE, or NOP
`define ST_RFRSH	5'd10		// NOP, wait tRFC - 1 cycle
`define ST_ILLDA0	5'd11		// NOP, data_timeout=1
`define ST_ILLDA1	5'd12		// NOP, data_timeout=0
`define ST_WRDATA0	5'd13		// NOP, wait tRCD - 1 cycle
`define ST_WRDATA1	5'd14		// WRITE, de=1
`define ST_WRDATA2	5'd15		// NOP, wait 3 cycles
`define ST_WRDATA3	5'd16		// NOP, ack=1, de=0
`define ST_WRDATA4	5'd17		// NOP, ack=0, wait 3 cycles
`define ST_RDDATA0	5'd18		// NOP, wait tRCD - 1 cycle
`define ST_RDDATA1	5'd19		// READ
`define ST_RDDATA2	5'd20		// NOP, ld=1, wait 4 cycles
`define ST_RDDATA3	5'd21		// NOP, ack=1, ld=0
`define ST_RDDATA4	5'd22		// NOP, ack=0, wait 4 cycles
`define ST_ILLIA0	5'd23		// NOP, inst_timeout=1
`define ST_ILLIA1	5'd24		// NOP, inst_timeout=0
`define ST_RDINST0	5'd25		// NOP, wait tRCD - 1 cycle
`define ST_RDINST1	5'd26		// READ
`define ST_RDINST2	5'd27		// NOP, ld=1, wait 4 cycles
`define ST_RDINST3	5'd28		// NOP, ack=1, ld=0
`define ST_RDINST4	5'd29		// NOP, ack=0, wait 4 cycles

`define T_INIT0		14'd10000	// min 100 usec with CKE = 0
`define T_INIT1		14'd10000	// min 100 usec with CKE = 1
`define T_RP		14'd2		// min 15 ns row precharge time
`define T_RFC		14'd7		// min 66 ns auto refresh period
`define T_MRD		14'd2		// min 14 ns load mode reg delay
`define T_RCD		14'd2		// 2 cycles active-to-rw delay

`define REFCNT		10'd780		// 8192 refresh cycles per 64 ms


module ramctrl(clk_ok, clk,
               inst_stb, inst_addr,
               inst_dout, inst_ack,
               inst_timeout,
               data_stb, data_we,
               data_addr, data_din,
               data_dout, data_ack,
               data_timeout);
    input clk_ok;
    input clk;
    input inst_stb;
    input [24:0] inst_addr;
    output [127:0] inst_dout;
    output reg inst_ack;
    output reg inst_timeout;
    input data_stb;
    input data_we;
    input [24:0] data_addr;
    input [127:0] data_din;
    output [127:0] data_dout;
    output reg data_ack;
    output reg data_timeout;

  wire sdram_clk;
  wire sdram_clk_aux;
  reg sdram_cke;
  reg sdram_cs_n;
  wire sdram_ras_n;
  wire sdram_cas_n;
  wire sdram_we_n;
  reg [1:0] sdram_ba;
  reg [12:0] sdram_a;
  wire [1:0] sdram_1_dqm;
  wire [15:0] sdram_1_dq;
  wire [1:0] sdram_2_dqm;
  wire [15:0] sdram_2_dq;

  reg [1:0] ram_cnt;
  reg [15:0] ram_1_dout;
  reg [15:0] ram_2_dout;
  reg ram_de;
  reg [127:0] data;
  reg data_ld;

  wire inst_addr_out_of_range;
  wire data_addr_out_of_range;

  reg [2:0] ram_cmd;
  reg ram_dqm;
  reg [13:0] count;
  reg [4:0] state;

  reg [9:0] refcnt;
  reg refflg;
  reg refrst;

  //
  // create ram instances
  //

  sdr sdr_1(
    .Clk(sdram_clk),
    .Cke(sdram_cke),
    .Cs_n(sdram_cs_n),
    .Ras_n(sdram_ras_n),
    .Cas_n(sdram_cas_n),
    .We_n(sdram_we_n),
    .Ba(sdram_ba[1:0]),
    .Addr(sdram_a[12:0]),
    .Dqm(sdram_1_dqm[1:0]),
    .Dq(sdram_1_dq[15:0])
  );

  sdr sdr_2(
    .Clk(sdram_clk),
    .Cke(sdram_cke),
    .Cs_n(sdram_cs_n),
    .Ras_n(sdram_ras_n),
    .Cas_n(sdram_cas_n),
    .We_n(sdram_we_n),
    .Ba(sdram_ba[1:0]),
    .Addr(sdram_a[12:0]),
    .Dqm(sdram_2_dqm[1:0]),
    .Dq(sdram_2_dq[15:0])
  );

  //
  // clock output to ram
  // the necessary phase shift will be accomplished by
  // a PLL if the controller is implemented on an FPGA
  //

  not #1 not_1(sdram_clk_aux, clk);
  not #1 not_2(sdram_clk, sdram_clk_aux);

  //
  // data output to ram
  //

  always @(*) begin
    case (ram_cnt[1:0])
      2'b00:
        begin
          ram_1_dout[15:0] = data_din[127:112];
          ram_2_dout[15:0] = data_din[111: 96];
        end
      2'b01:
        begin
          ram_1_dout[15:0] = data_din[ 95: 80];
          ram_2_dout[15:0] = data_din[ 79: 64];
        end
      2'b10:
        begin
          ram_1_dout[15:0] = data_din[ 63: 48];
          ram_2_dout[15:0] = data_din[ 47: 32];
        end
      2'b11:
        begin
          ram_1_dout[15:0] = data_din[ 31: 16];
          ram_2_dout[15:0] = data_din[ 15:  0];
        end
    endcase
  end

  assign sdram_1_dq[15:0] = ram_de ? ram_1_dout[15:0] : 16'hzzzz;
  assign sdram_2_dq[15:0] = ram_de ? ram_2_dout[15:0] : 16'hzzzz;

  //
  // data output to cache
  //

  always @(posedge clk) begin
    if (data_ld & (ram_cnt[1:0] == 2'b00)) begin
      data[127:112] <= sdram_1_dq[15:0];
      data[111: 96] <= sdram_2_dq[15:0];
    end
    if (data_ld & (ram_cnt[1:0] == 2'b01)) begin
      data[ 95: 80] <= sdram_1_dq[15:0];
      data[ 79: 64] <= sdram_2_dq[15:0];
    end
    if (data_ld & (ram_cnt[1:0] == 2'b10)) begin
      data[ 63: 48] <= sdram_1_dq[15:0];
      data[ 47: 32] <= sdram_2_dq[15:0];
    end
    if (data_ld & (ram_cnt[1:0] == 2'b11)) begin
      data[ 31: 16] <= sdram_1_dq[15:0];
      data[ 15:  0] <= sdram_2_dq[15:0];
    end
  end

  assign inst_dout[127:0] = data[127:0];
  assign data_dout[127:0] = data[127:0];

  //
  // address range check
  //

  assign inst_addr_out_of_range = | inst_addr[24:23];
  assign data_addr_out_of_range = | data_addr[24:23];

  //
  // ramctrl state machine
  //

  assign sdram_ras_n = ram_cmd[2];
  assign sdram_cas_n = ram_cmd[1];
  assign sdram_we_n = ram_cmd[0];

  assign sdram_1_dqm[1] = ram_dqm;
  assign sdram_1_dqm[0] = ram_dqm;
  assign sdram_2_dqm[1] = ram_dqm;
  assign sdram_2_dqm[0] = ram_dqm;

  always @(posedge clk or negedge clk_ok) begin
    // asynchronous reset
    if (~clk_ok) begin
      inst_ack <= 0;
      inst_timeout <= 0;
      data_ack <= 0;
      data_timeout <= 0;
      sdram_cke <= 0;
      sdram_cs_n <= 1;
      ram_cnt <= 0;
      ram_de <= 0;
      data_ld <= 0;
      ram_cmd <= `CMD_NOP;
      ram_dqm <= 1;
      count <= `T_INIT0 - 14'd1;
      state <= `ST_INIT0;
      refrst <= 0;
    end else begin
      if (| count[13:0]) begin
        // wait until count = 0
        // if count is loaded with N on a state transition, the
        // new state will last for (N+1) clock cycles before (!)
        // any action specified in the new state will take place
        count <= count - 14'd1;
        // ram_cnt cycles through the 16-bit half-words in SDRAM
        // during burst read/writes while the main state machine
        // waits, thus it must be incremented here
        ram_cnt <= ram_cnt + 2'd1;
      end else begin
        case (state)
          //----------------------------
          // init
          //----------------------------
          `ST_INIT0:
            begin
              sdram_cke <= 1;
              sdram_cs_n <= 0;
              ram_cmd <= `CMD_NOP;
              count <= `T_INIT1 - 14'd1;
              state <= `ST_INIT1;
            end
          `ST_INIT1:
            begin
              ram_cmd <= `CMD_PRCHG;
              sdram_ba <= 2'b00;	// don't care
              sdram_a <= 13'h0400;	// precharge all
              state <= `ST_INIT2;
            end
          `ST_INIT2:
            begin
              ram_cmd <= `CMD_NOP;
              count <= `T_RP - 14'd2;
              state <= `ST_INIT3;
            end
          `ST_INIT3:
            begin
              ram_cmd <= `CMD_ARFRS;
              state <= `ST_INIT4;
            end
          `ST_INIT4:
            begin
              ram_cmd <= `CMD_NOP;
              count <= `T_RFC - 14'd2;
              state <= `ST_INIT5;
            end
          `ST_INIT5:
            begin
              ram_cmd <= `CMD_ARFRS;
              state <= `ST_INIT6;
            end
          `ST_INIT6:
            begin
              ram_cmd <= `CMD_NOP;
              count <= `T_RFC - 14'd2;
              state <= `ST_INIT7;
            end
          `ST_INIT7:
            begin
              ram_cmd <= `CMD_MRSET;
              sdram_ba <= 2'b00;
              sdram_a <= `MODE;
              state <= `ST_INIT8;
            end
          `ST_INIT8:
            begin
              ram_cmd <= `CMD_NOP;
              ram_dqm <= 0;
              count <= `T_MRD - 14'd2;
              state <= `ST_IDLE;
            end
          //----------------------------
          // idle
          //----------------------------
          `ST_IDLE:
            begin
              if (refflg) begin
                // refresh request
                ram_cmd <= `CMD_ARFRS;
                state <= `ST_RFRSH;
                refrst <= 1;
              end else begin
                if (data_stb) begin
                  if (data_addr_out_of_range) begin
                    // illegal data address
                    ram_cmd <= `CMD_NOP;
                    state <= `ST_ILLDA0;
                  end else begin
                    // data address is ok
                    if (data_we) begin
                      // data write request
                      ram_cmd <= `CMD_ACTV;
                      sdram_ba <= data_addr[22:21];
                      sdram_a <= data_addr[20:8];
                      state <= `ST_WRDATA0;
                    end else begin
                      // data read request
                      ram_cmd <= `CMD_ACTV;
                      sdram_ba <= data_addr[22:21];
                      sdram_a <= data_addr[20:8];
                      state <= `ST_RDDATA0;
                    end
                  end
                end else begin
                  if (inst_stb) begin
                    if (inst_addr_out_of_range) begin
                      // illegal inst address
                      ram_cmd <= `CMD_NOP;
                      state <= `ST_ILLIA0;
                    end else begin
                      // inst address is ok
                      // inst read request
                      ram_cmd <= `CMD_ACTV;
                      sdram_ba <= inst_addr[22:21];
                      sdram_a <= inst_addr[20:8];
                      state <= `ST_RDINST0;
                    end
                  end else begin
                    // no request
                    ram_cmd <= `CMD_NOP;
                    state <= `ST_IDLE;
                  end
                end
              end
            end
          //----------------------------
          // refresh
          //----------------------------
          `ST_RFRSH:
            begin
              ram_cmd <= `CMD_NOP;
              count <= `T_RFC - 14'd2;
              state <= `ST_IDLE;
              refrst <= 0;
            end
          //----------------------------
          // illegal data address
          //----------------------------
          `ST_ILLDA0:
            begin
              data_timeout <= 1;
              ram_cmd <= `CMD_NOP;
              state <= `ST_ILLDA1;
            end
          `ST_ILLDA1:
            begin
              data_timeout <= 0;
              ram_cmd <= `CMD_NOP;
              state <= `ST_IDLE;
            end
          //----------------------------
          // write data
          //----------------------------
          `ST_WRDATA0:
            begin
              ram_cmd <= `CMD_NOP;
              count <= `T_RCD - 14'd2;
              state <= `ST_WRDATA1;
            end
          `ST_WRDATA1:
            begin
              ram_cnt <= 0;
              ram_de <= 1;
              ram_cmd <= `CMD_WRITE;
              sdram_ba <= data_addr[22:21];
              sdram_a <= { 3'b001, data_addr[7:0], 2'b00 };
              state <= `ST_WRDATA2;
            end
          `ST_WRDATA2:
            begin
              ram_cnt <= ram_cnt + 2'd1;
              ram_cmd <= `CMD_NOP;
              count <= 2;
              state <= `ST_WRDATA3;
            end
          `ST_WRDATA3:
            begin
              data_ack <= 1;
              ram_de <= 0;
              ram_cmd <= `CMD_NOP;
              state <= `ST_WRDATA4;
            end
          `ST_WRDATA4:
            begin
              data_ack <= 0;
              ram_cmd <= `CMD_NOP;
              count <= 2;
              state <= `ST_IDLE;
            end
          //----------------------------
          // read data
          //----------------------------
          `ST_RDDATA0:
            begin
              ram_cmd <= `CMD_NOP;
              count <= `T_RCD - 14'd2;
              state <= `ST_RDDATA1;
            end
          `ST_RDDATA1:
            begin
              ram_cmd <= `CMD_READ;
              sdram_ba <= data_addr[22:21];
              sdram_a <= { 3'b001, data_addr[7:0], 2'b00 };
              state <= `ST_RDDATA2;
            end
          `ST_RDDATA2:
            begin
              ram_cnt <= 0;
              data_ld <= 1;
              ram_cmd <= `CMD_NOP;
              count <= 3;
              state <= `ST_RDDATA3;
            end
          `ST_RDDATA3:
            begin
              data_ack <= 1;
              data_ld <= 0;
              ram_cmd <= `CMD_NOP;
              state <= `ST_RDDATA4;
            end
          `ST_RDDATA4:
            begin
              data_ack <= 0;
              ram_cmd <= `CMD_NOP;
              count <= 3;
              state <= `ST_IDLE;
            end
          //----------------------------
          // illegal inst address
          //----------------------------
          `ST_ILLIA0:
            begin
              inst_timeout <= 1;
              ram_cmd <= `CMD_NOP;
              state <= `ST_ILLIA1;
            end
          `ST_ILLIA1:
            begin
              inst_timeout <= 0;
              ram_cmd <= `CMD_NOP;
              state <= `ST_IDLE;
            end
          //----------------------------
          // read inst
          //----------------------------
          `ST_RDINST0:
            begin
              ram_cmd <= `CMD_NOP;
              count <= `T_RCD - 14'd2;
              state <= `ST_RDINST1;
            end
          `ST_RDINST1:
            begin
              ram_cmd <= `CMD_READ;
              sdram_ba <= inst_addr[22:21];
              sdram_a <= { 3'b001, inst_addr[7:0], 2'b00 };
              state <= `ST_RDINST2;
            end
          `ST_RDINST2:
            begin
              ram_cnt <= 0;
              data_ld <= 1;
              ram_cmd <= `CMD_NOP;
              count <= 3;
              state <= `ST_RDINST3;
            end
          `ST_RDINST3:
            begin
              inst_ack <= 1;
              data_ld <= 0;
              ram_cmd <= `CMD_NOP;
              state <= `ST_RDINST4;
            end
          `ST_RDINST4:
            begin
              inst_ack <= 0;
              ram_cmd <= `CMD_NOP;
              count <= 3;
              state <= `ST_IDLE;
            end
          //----------------------------
          // not used
          //----------------------------
          default:
            begin
              inst_ack <= 0;
              inst_timeout <= 0;
              data_ack <= 0;
              data_timeout <= 0;
              sdram_cke <= 0;
              sdram_cs_n <= 1;
              ram_cnt <= 0;
              ram_de <= 0;
              data_ld <= 0;
              ram_cmd <= `CMD_NOP;
              ram_dqm <= 1;
              count <= `T_INIT0 - 14'd1;
              state <= `ST_INIT0;
              refrst <= 0;
            end
        endcase
      end
    end
  end

  //
  // refresh counter
  //

  always @(posedge clk or negedge clk_ok) begin
    if (~clk_ok) begin
      refcnt <= 10'd0;
    end else begin
      if (refcnt == 10'd0) begin
        refcnt <= `REFCNT;
        refflg <= 1;
      end else begin
        refcnt <= refcnt - 10'd1;
        if (refrst) begin
          refflg <= 0;
        end
      end
    end
  end

endmodule
