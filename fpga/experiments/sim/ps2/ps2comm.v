//
// ps2comm.v -- simulate PS/2 communication
//


`timescale 1ns/10ps
`default_nettype none


module ps2comm;

  reg clk;			// system clock
  reg rst_in;			// reset, input
  reg rst;			// system reset

  wire ps2_clock;		// PS/2 clock line
  wire ps2_data;		// PS/2 data line

  wire [7:0] device_rcv_data;	// data received by device
  wire device_rcv_error;	// error indicator for data
  wire device_rcv_strobe;	// qualifies the two preceding signals
  wire device_xmt_ready;	// device accepts data to be transmitted
  reg [7:0] device_xmt_data;	// data to be transmitted by device
  reg device_xmt_strobe;	// start transmission

  wire [7:0] host_rcv_data;	// data received by host
  wire host_rcv_error;		// error indicator for data
  wire host_rcv_strobe;		// qualifies the two preceding signals
  wire host_xmt_ready;		// host accepts data to be transmitted
  reg [7:0] host_xmt_data;	// data to be transmitted by host
  reg host_xmt_strobe;		// start transmission

  //
  // simulation control
  //

  initial begin
    #0		$timeformat(-9, 1, " ns", 12);
		$dumpfile("dump.vcd");
		$dumpvars(0, ps2comm);
		clk = 1;
		rst_in = 1;
		device_xmt_data[7:0] = 8'hxx;
		device_xmt_strobe = 0;
		host_xmt_data[7:0] = 8'hxx;
		host_xmt_strobe = 0;
    #73		rst_in = 0;
    #250090	device_xmt_data[7:0] = 8'hA7;
		device_xmt_strobe = 1;
    #20		device_xmt_data[7:0] = 8'hxx;
		device_xmt_strobe = 0;
    #1200000	device_xmt_data[7:0] = 8'h95;
		device_xmt_strobe = 1;
    #20		device_xmt_data[7:0] = 8'hxx;
		device_xmt_strobe = 0;
    #1200000	host_xmt_data[7:0] = 8'h58;
		host_xmt_strobe = 1;
    #20		host_xmt_data[7:0] = 8'hxx;
		host_xmt_strobe = 0;
    #1200000	host_xmt_data[7:0] = 8'h6A;
		host_xmt_strobe = 1;
    #20		host_xmt_data[7:0] = 8'hxx;
		host_xmt_strobe = 0;
    #1200000	$finish;
  end

  //
  // clock generator
  //

  always begin
    #10 clk = ~clk;		// 20 nsec cycle time
  end

  //
  // reset synchronizer
  //

  always @(posedge clk) begin
    rst <= rst_in;
  end

  //
  // module instantiations
  //

  pullup(ps2_clock);
  pullup(ps2_data);

  device device_0(
    .clk(clk),
    .rst(rst),
    .rcv_data(device_rcv_data[7:0]),
    .rcv_error(device_rcv_error),
    .rcv_strobe(device_rcv_strobe),
    .xmt_ready(device_xmt_ready),
    .xmt_data(device_xmt_data[7:0]),
    .xmt_strobe(device_xmt_strobe),
    .ps2_clock(ps2_clock),
    .ps2_data(ps2_data)
  );

  host host_0(
    .clk(clk),
    .rst(rst),
    .rcv_data(host_rcv_data[7:0]),
    .rcv_error(host_rcv_error),
    .rcv_strobe(host_rcv_strobe),
    .xmt_ready(host_xmt_ready),
    .xmt_data(host_xmt_data[7:0]),
    .xmt_strobe(host_xmt_strobe),
    .ps2_clock(ps2_clock),
    .ps2_data(ps2_data)
  );

endmodule
