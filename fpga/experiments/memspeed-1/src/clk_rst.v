//
// clk_rst.v -- clock and reset generator
//


`timescale 1ns/10ps
`default_nettype none


module clk_rst(clk_in, rst_inout_n,
               sdram_clk, sdram_fb,
               clk, clk_ok, rst);
    input clk_in;
    inout rst_inout_n;
    output sdram_clk;
    input sdram_fb;
    output clk;
    output clk_ok;
    output rst;

  wire clk_in_buf;
  wire int_clk;
  wire int_locked;
  wire ext_rst_n;
  wire ext_fb;
  wire ext_locked;

  reg rst_p_n;
  reg rst_s_n;
  reg [23:0] rst_counter;
  wire rst_counting;

  //------------------------------------------------------------

  IBUFG clk_in_buffer(
    .I(clk_in),
    .O(clk_in_buf)
  );

  DCM int_dcm(
    .CLKIN(clk_in_buf),
    .CLKFB(clk),
    .RST(1'b0),
    .CLK0(int_clk),
    .LOCKED(int_locked)
  );

  BUFG int_clk_buffer(
    .I(int_clk),
    .O(clk)
  );

  //------------------------------------------------------------

  SRL16 ext_dll_rst_gen(
    .CLK(clk_in_buf),
    .D(int_locked),
    .Q(ext_rst_n),
    .A0(1'b1),
    .A1(1'b1),
    .A2(1'b1),
    .A3(1'b1)
  );

  defparam ext_dll_rst_gen.INIT = 16'h0000;

  //------------------------------------------------------------

  IBUFG ext_fb_buffer(
    .I(sdram_fb),
    .O(ext_fb)
  );

  DCM ext_dcm(
    .CLKIN(clk_in_buf),
    .CLKFB(ext_fb),
    .RST(~ext_rst_n),
    .CLK0(sdram_clk),
    .LOCKED(ext_locked)
  );

  assign clk_ok = int_locked & ext_locked;

  //------------------------------------------------------------

  assign rst_counting = (rst_counter == 24'hFFFFFF) ? 0 : 1;
  assign rst_inout_n = (rst_counter[23] == 0) ? 1'b0 : 1'bz;

  always @(posedge clk_in_buf) begin
    rst_p_n <= rst_inout_n;
    rst_s_n <= rst_p_n;
    if (rst_counting) begin
      rst_counter <= rst_counter + 1;
    end else begin
      if (~rst_s_n | ~clk_ok) begin
        rst_counter <= 24'h000000;
      end
    end
  end

  assign rst = rst_counting;

endmodule
