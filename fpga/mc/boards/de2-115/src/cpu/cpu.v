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

endmodule
