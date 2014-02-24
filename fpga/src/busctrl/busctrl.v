//
// busctrl.v -- bus controller
//

module busctrl(cpu_en, cpu_wr, cpu_size, cpu_addr,
               cpu_data_out, cpu_data_in, cpu_wt,
               ram_en, ram_wr, ram_size, ram_addr,
               ram_data_in, ram_data_out, ram_wt,
               rom_en, rom_wr, rom_size, rom_addr,
               rom_data_out, rom_wt,
               tmr_en, tmr_wr, tmr_addr,
               tmr_data_in, tmr_data_out, tmr_wt,
               dsp_en, dsp_wr, dsp_addr,
               dsp_data_in, dsp_data_out, dsp_wt,
               kbd_en, kbd_wr, kbd_addr,
               kbd_data_in, kbd_data_out, kbd_wt,
               ser0_en, ser0_wr, ser0_addr,
               ser0_data_in, ser0_data_out, ser0_wt,
               ser1_en, ser1_wr, ser1_addr,
               ser1_data_in, ser1_data_out, ser1_wt,
               dsk_en, dsk_wr, dsk_addr,
               dsk_data_in, dsk_data_out, dsk_wt);
    // cpu
    input cpu_en;
    input cpu_wr;
    input [1:0] cpu_size;
    input [31:0] cpu_addr;
    input [31:0] cpu_data_out;
    output [31:0] cpu_data_in;
    output cpu_wt;
    // ram
    output ram_en;
    output ram_wr;
    output [1:0] ram_size;
    output [24:0] ram_addr;
    output [31:0] ram_data_in;
    input [31:0] ram_data_out;
    input ram_wt;
    // rom
    output rom_en;
    output rom_wr;
    output [1:0] rom_size;
    output [20:0] rom_addr;
    input [31:0] rom_data_out;
    input rom_wt;
    // tmr
    output tmr_en;
    output tmr_wr;
    output [3:2] tmr_addr;
    output [31:0] tmr_data_in;
    input [31:0] tmr_data_out;
    input tmr_wt;
    // dsp
    output dsp_en;
    output dsp_wr;
    output [13:2] dsp_addr;
    output [15:0] dsp_data_in;
    input [15:0] dsp_data_out;
    input dsp_wt;
    // kbd
    output kbd_en;
    output kbd_wr;
    output kbd_addr;
    output [7:0] kbd_data_in;
    input [7:0] kbd_data_out;
    input kbd_wt;
    // ser0
    output ser0_en;
    output ser0_wr;
    output [3:2] ser0_addr;
    output [7:0] ser0_data_in;
    input [7:0] ser0_data_out;
    input ser0_wt;
    // ser1
    output ser1_en;
    output ser1_wr;
    output [3:2] ser1_addr;
    output [7:0] ser1_data_in;
    input [7:0] ser1_data_out;
    input ser1_wt;
    // dsk
    output dsk_en;
    output dsk_wr;
    output [19:2] dsk_addr;
    output [31:0] dsk_data_in;
    input [31:0] dsk_data_out;
    input dsk_wt;

  wire i_o_en;

  //
  // address decoder
  //
  // RAM: architectural limit = 512 MB
  //      board limit         =  32 MB
  assign ram_en =
    (cpu_en == 1 && cpu_addr[31:29] == 3'b000
                 && cpu_addr[28:25] == 4'b0000) ? 1 : 0;
  // ROM: architectural limit = 256 MB
  //      board limit         =   2 MB
  assign rom_en =
    (cpu_en == 1 && cpu_addr[31:28] == 4'b0010
                 && cpu_addr[27:21] == 7'b0000000) ? 1 : 0;
  // I/O: architectural limit = 256 MB
  assign i_o_en =
    (cpu_en == 1 && cpu_addr[31:28] == 4'b0011) ? 1 : 0;
  assign tmr_en =
    (i_o_en == 1 && cpu_addr[27:20] == 8'h00) ? 1 : 0;
  assign dsp_en =
    (i_o_en == 1 && cpu_addr[27:20] == 8'h01) ? 1 : 0;
  assign kbd_en =
    (i_o_en == 1 && cpu_addr[27:20] == 8'h02) ? 1 : 0;
  assign ser0_en =
    (i_o_en == 1 && cpu_addr[27:20] == 8'h03
                 && cpu_addr[19:12] == 8'h00) ? 1 : 0;
  assign ser1_en =
    (i_o_en == 1 && cpu_addr[27:20] == 8'h03
                 && cpu_addr[19:12] == 8'h01) ? 1 : 0;
  assign dsk_en =
    (i_o_en == 1 && cpu_addr[27:20] == 8'h04) ? 1 : 0;

  // to cpu
  assign cpu_wt =
    (ram_en == 1) ? ram_wt :
    (rom_en == 1) ? rom_wt :
    (tmr_en == 1) ? tmr_wt :
    (dsp_en == 1) ? dsp_wt :
    (kbd_en == 1) ? kbd_wt :
    (ser0_en == 1) ? ser0_wt :
    (ser1_en == 1) ? ser1_wt :
    (dsk_en == 1) ? dsk_wt :
    1;
  assign cpu_data_in[31:0] =
    (ram_en == 1) ? ram_data_out[31:0] :
    (rom_en == 1) ? rom_data_out[31:0] :
    (tmr_en == 1) ? tmr_data_out[31:0] :
    (dsp_en == 1) ? { 16'h0000, dsp_data_out[15:0] } :
    (kbd_en == 1) ? { 24'h000000, kbd_data_out[7:0] } :
    (ser0_en == 1) ? { 24'h000000, ser0_data_out[7:0] } :
    (ser1_en == 1) ? { 24'h000000, ser1_data_out[7:0] } :
    (dsk_en == 1) ? dsk_data_out[31:0] :
    32'h00000000;

  // to ram
  assign ram_wr = cpu_wr;
  assign ram_size[1:0] = cpu_size[1:0];
  assign ram_addr[24:0] = cpu_addr[24:0];
  assign ram_data_in[31:0] = cpu_data_out[31:0];

  // to rom
  assign rom_wr = cpu_wr;
  assign rom_size[1:0] = cpu_size[1:0];
  assign rom_addr[20:0] = cpu_addr[20:0];

  // to tmr
  assign tmr_wr = cpu_wr;
  assign tmr_addr[3:2] = cpu_addr[3:2];
  assign tmr_data_in[31:0] = cpu_data_out[31:0];

  // to dsp
  assign dsp_wr = cpu_wr;
  assign dsp_addr[13:2] = cpu_addr[13:2];
  assign dsp_data_in[15:0] = cpu_data_out[15:0];

  // to kbd
  assign kbd_wr = cpu_wr;
  assign kbd_addr = cpu_addr[2];
  assign kbd_data_in[7:0] = cpu_data_out[7:0];

  // to ser0
  assign ser0_wr = cpu_wr;
  assign ser0_addr[3:2] = cpu_addr[3:2];
  assign ser0_data_in[7:0] = cpu_data_out[7:0];

  // to ser1
  assign ser1_wr = cpu_wr;
  assign ser1_addr[3:2] = cpu_addr[3:2];
  assign ser1_data_in[7:0] = cpu_data_out[7:0];

  // to dsk
  assign dsk_wr = cpu_wr;
  assign dsk_addr[19:2] = cpu_addr[19:2];
  assign dsk_data_in[31:0] = cpu_data_out[31:0];

endmodule
