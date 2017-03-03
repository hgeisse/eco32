//
// clk_rst.v -- clock and reset generator
//


`timescale 1ns/10ps
`default_nettype none


module clk_rst(clk_in, rst_inout_n,
               sdram_clk, sdram_fb,
               clk_ok, clk2, clk, rst);
    input clk_in;
    inout rst_inout_n;
    output sdram_clk;
    input sdram_fb;
    output clk_ok;
    output clk2;
    output clk;
    output rst;

  wire clk_in_buf;
  wire int_clk2;
  wire int_clk;
  wire int_locked;
  wire ext_rst_n;
  wire ext_fb;
  wire ext_clk2;
  wire ext_locked;

  wire rst_counting;
  reg [23:0] rst_counter;
  reg rst_p_n;
  reg rst_s_n;

  //
  // internal DCM, 100 MHz and 50 MHz
  //

  IBUF clk_in_buffer(
    .I(clk_in),
    .O(clk_in_buf)
  );

  DCM int_dcm(
    .CLKIN(clk_in_buf),
    .CLKFB(clk2),
    .RST(1'b0),
    .CLK0(int_clk2),
    .CLKDV(int_clk),
    .LOCKED(int_locked)
  );

  defparam int_dcm.CLKDV_DIVIDE = 2;

  BUFG int_clk2_buffer(
    .I(int_clk2),
    .O(clk2)
  );

  BUFG int_clk_buffer(
    .I(int_clk),
    .O(clk)
  );

  //
  // reset circuit for external DCM
  //

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

  //
  // external DCM, 100 MHz
  //

  IBUF ext_fb_buffer(
    .I(sdram_fb),
    .O(ext_fb)
  );

  DCM ext_dcm(
    .CLKIN(clk_in_buf),
    .CLKFB(ext_fb),
    .RST(~ext_rst_n),
    .CLK0(ext_clk2),
    .LOCKED(ext_locked),
    .PSEN(1'b0),
    .PSCLK(1'b0),
    .PSINCDEC(1'b0)
  );

  defparam ext_dcm.CLKOUT_PHASE_SHIFT = "FIXED";
  defparam ext_dcm.PHASE_SHIFT = 0;

  OBUF ext_clk2_buffer(
    .I(ext_clk2),
    .O(sdram_clk)
  );

  assign clk_ok = int_locked & ext_locked;

  //
  // reset generator
  //

  assign rst_counting = (rst_counter == 24'hFFFFFF) ? 0 : 1;
  assign rst_inout_n = (rst_counter[23] == 0) ? 1'b0 : 1'bz;

  always @(posedge clk2) begin
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
