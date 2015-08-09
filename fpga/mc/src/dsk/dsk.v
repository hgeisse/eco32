//
// dsk.v -- parallel ATA (IDE) disk interface
//


`timescale 1ns/10ps
`default_nettype none


module dsk(clk, rst,
           stb, we, addr,
           data_in, data_out,
           ack, irq,
           ata_d, ata_a, ata_cs0_n, ata_cs1_n,
           ata_dior_n, ata_diow_n, ata_intrq,
           ata_dmarq, ata_dmack_n, ata_iordy);
    // internal interface signals
    input clk;
    input rst;
    input stb;
    input we;
    input [19:2] addr;
    input [31:0] data_in;
    output [31:0] data_out;
    output ack;
    output irq;
    // external interface signals
    inout [15:0] ata_d;
    output [2:0] ata_a;
    output ata_cs0_n, ata_cs1_n;
    output ata_dior_n, ata_diow_n;
    input ata_intrq;
    input ata_dmarq;
    output ata_dmack_n;
    input ata_iordy;

  wire bus_wait;

  ata_ctrl ata_ctrl_1(
    .clk(clk),
    .reset(rst),
    .bus_en(stb),
    .bus_wr(we),
    .bus_addr(addr),
    .bus_din(data_in),
    .bus_dout(data_out),
    .bus_wait(bus_wait),
    .bus_irq(irq),
    .ata_d(ata_d),
    .ata_a(ata_a),
    .ata_cs0_n(ata_cs0_n),
    .ata_cs1_n(ata_cs1_n),
    .ata_dior_n(ata_dior_n),
    .ata_diow_n(ata_diow_n),
    .ata_intrq(ata_intrq),
    .ata_dmarq(ata_dmarq),
    .ata_dmack_n(ata_dmack_n),
    .ata_iordy(ata_iordy)
  );

  assign ack = stb & ~bus_wait;

endmodule
