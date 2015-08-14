//
// cpu.v -- the ECO32 CPU
//


`timescale 1ns/10ps
`default_nettype none


module cpu(clk, rst,
           bus_stb, bus_we, bus_addr,
           bus_din, bus_dout, bus_ack,
           bus_irq);
    input clk;				// system clock
    input rst;				// system reset
    output bus_stb;			// bus strobe
    output bus_we;			// bus write enable
    output [31:2] bus_addr;		// bus address (word address)
    input [31:0] bus_din;		// bus data input, for reads
    output [31:0] bus_dout;		// bus data output, for writes
    input bus_ack;			// bus acknowledge
    input [15:0] bus_irq;		// bus interrupt requests

  wire cpu_stb;
  wire cpu_we;
  wire [1:0] cpu_size;
  wire [31:0] cpu_addr;
  wire [31:0] cpu_din;
  wire [31:0] cpu_dout;
  wire cpu_ack;
  wire [15:0] cpu_irq;

  cpu_bus cpu_bus_1(
    .clk(clk),
    .rst(rst),
    .bus_stb(bus_stb),
    .bus_we(bus_we),
    .bus_addr(bus_addr[31:2]),
    .bus_din(bus_din[31:0]),
    .bus_dout(bus_dout[31:0]),
    .bus_ack(bus_ack),
    .bus_irq(bus_irq[15:0]),
    .cpu_stb(cpu_stb),
    .cpu_we(cpu_we),
    .cpu_size(cpu_size[1:0]),
    .cpu_addr(cpu_addr[31:0]),
    .cpu_din(cpu_din[31:0]),
    .cpu_dout(cpu_dout[31:0]),
    .cpu_ack(cpu_ack),
    .cpu_irq(cpu_irq[15:0])
  );

  cpu_core cpu_core_1(
    .clk(clk),
    .rst(rst),
    .bus_stb(cpu_stb),
    .bus_we(cpu_we),
    .bus_size(cpu_size[1:0]),
    .bus_addr(cpu_addr[31:0]),
    .bus_din(cpu_din[31:0]),
    .bus_dout(cpu_dout[31:0]),
    .bus_ack(cpu_ack),
    .bus_irq(cpu_irq[15:0])
  );

endmodule
