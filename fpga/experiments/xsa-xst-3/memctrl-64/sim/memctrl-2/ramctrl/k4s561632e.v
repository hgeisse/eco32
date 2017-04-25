/*=============================================================================
* Copyright(c) Samsung Electronics Co., 1997,1998. All rights reseved.
*
* Verilog Behavioral Model of Synchronous 256 SDRAM
*
* Description : This is a synchrounous high data rate DRAM,
*               fabricated with SAMSUNG's high performance
*               CMOS technology.
*
* Revision No. : V4.5
* date         : Jan. 21. 2002
*
*===============================================================================*/

`define M256
`define LVTTL
`define NBANK4
`define X16
`define S75
`define v

`ifdef LVTTL //M256+G6
   `ifdef X16 
	`ifdef S75 
	    `define tCCmin  7.5    	// clock minimum cycle time
	    `define tCCmax  1000        // clock maximun cycle time
	    `define tCC3    7.5         // clock minimun cycle time at cas latency=3
	    `define tCC2    10          // clock minimun cycle time at cas latency=2
	    `define tCH     2.5         // clock high pulse width
	    `define tCL     2.5         // clock low pulse width
	    `define tSS     1.5         // input setup time
	    `define tSH     0.8         // input hold time
	    `define tRRD    15          // row to row delay(min)
	    `define tRCD    20          // ras to cas delay(min)
	    `define tRP     20          // row precharge time(min)
	    `define tRASmin 45          // row active minimum time
	    `define tRASmax 100000  	// row active maximum time
	    `define tRC     65          // row cycle time(min)
	    `define tCCD    7.5          // col. address to col. address delay:
	    `define tSAC2   6
	    `define tSAC3   5.4
	    `define tSHZ2   6
	    `define tSHZ3   5.4
	`endif //end of S75
	`ifdef S60 
	    `define tCCmin  6    	// clock minimum cycle time
	    `define tCCmax  1000        // clock maximun cycle time
	    `define tCC3    6           // clock minimun cycle time at cas latency=3
	    `define tCH     2.5         // clock high pulse width
	    `define tCL     2.5         // clock low pulse width
	    `define tSS     1.5           // input setup time
	    `define tSH     1           // input hold time
	    `define tRRD    12          // row to row delay(min)
	    `define tRCD    18          // ras to cas delay(min)
	    `define tRP     18          // row precharge time(min)
	    `define tRASmin 42          // row active minimum time
	    `define tRASmax 100000  	// row active maximum time
	    `define tRC     60          // row cycle time(min)
	    `define tCCD    6           // col. address to col. address delay:
	    `define tSAC3   5
	    `define tSHZ3   5
	`endif //end of S60
   `endif //end of X16
   `ifdef X8 
        `ifdef S75 
            `define tCCmin  7.5         // clock minimum cycle time
            `define tCCmax  1000        // clock maximun cycle time
            `define tCC3    7.5         // clock minimun cycle time at cas latency=3
            `define tCC2    10          // clock minimun cycle time at cas latency=2
            `define tCH     2.5         // clock high pulse width
            `define tCL     2.5         // clock low pulse width
            `define tSS     1.5         // input setup time
            `define tSH     0.8         // input hold time
            `define tRRD    15          // row to row delay(min)
            `define tRCD    20          // ras to cas delay(min)
            `define tRP     20          // row precharge time(min)
            `define tRASmin 45          // row active minimum time
            `define tRASmax 100000      // row active maximum time
            `define tRC     65          // row cycle time(min)
            `define tCCD    7.5          // col. address to col. address delay:
            `define tSAC2   6
            `define tSAC3   5.4
            `define tSHZ2   6
            `define tSHZ3   5.4
        `endif //end of S75
   `endif //end of X8
   `ifdef X4 
        `ifdef S75 
            `define tCCmin  7.5         // clock minimum cycle time
            `define tCCmax  1000        // clock maximun cycle time
            `define tCC3    7.5         // clock minimun cycle time at cas latency=3
            `define tCC2    10          // clock minimun cycle time at cas latency=2
            `define tCH     2.5         // clock high pulse width
            `define tCL     2.5         // clock low pulse width
            `define tSS     1.5         // input setup time
            `define tSH     0.8         // input hold time
            `define tRRD    15          // row to row delay(min)
            `define tRCD    20          // ras to cas delay(min)
            `define tRP     20          // row precharge time(min)
            `define tRASmin 45          // row active minimum time
            `define tRASmax 100000      // row active maximum time
            `define tRC     65          // row cycle time(min)
            `define tCCD    7.5          // col. address to col. address delay:
            `define tSAC2   6
            `define tSAC3   5.4
            `define tSHZ2   6
            `define tSHZ3   5.4
        `endif //end of S75
   `endif //end of X4
`endif //end of LVTTL


/*==================================================================
 * Copyright(c) Samsung Electronics Co., 1997,1998. All rights reseved.
 *
 * Verilog Behavioral Model of Synchronous DRAM 
 *
 * Device: -  16M SDRAM(2nd Gen., 3rd Gen., 4th Gen., 5th Gen.) 
 *	   -  64M SDRAM(2nd Gen., 3rd Gen., 4th Gen., 5th Gen.)
 *	   - 128M SDRAM(1st Gen., 2nd Gen.,)
 *	   - 256M SDRAM(2nd Gen.)
 *
 * Description : This is a synchrounous high data rate DRAM, 
 *		 fabricated with SAMSUNG's high performance  
 *               CMOS technology.                                
 *
 * Developer   : Jae-Ha Kim.
 *  		 CAE Team. Semiconductor R&D Centor. 
 *		 Semiconductor Division. Samsung Electronics Co.
 *
 * Revision    : Yong-Cheol Bae. ( e-mail: jjbae@samsung.co.kr )
 * 1.0~2.3	 DRAM2 Design. Semiconductor R&D Centor. 
 *		 Semiconductor Division. Samsung Electronics Co.
 * Revision    : Kyung-Woo Nam. ( e-mail: aboverim@samsung.co.kr )
 * 3.0~		 DRAM2 Design. Semiconductor R&D Centor. 
 *		 Semiconductor Division. Samsung Electronics Co.
 *==================================================================
 * Revision No. : V2.3 
 * date		: Aug. 20. 1997
 *		: Oct. 22. 1997
 *		: Jan. 15. 1998   - 16M 4Gen. & 64M 3Gen. spec update
 *		: Jun. 30. 1998   - 16M 4Gen. & 64M 3Gen. & 2Mx32 & 
 *		                  128M 1Gen. & 256M 2Gen. AC spec update
 *		: Oct. 08. 1998   - all-bank precharge revision
 *		                  bank note change: ba0=MSB,ba1=LSB @ 4banks
 *		: Nov. 18. 1998   - option set for verbose (e.g. +define+v )
 *		                  128M 2Gen. AC spec added. 
 *                                conformed model through verilog-XL 2.6.27 
 * Revision No. : V3.0 
 * date		: Apr. 07. 1999   - tRDL=2
 *			1. parameterize auto-precharge address : ADDR_AP
 *			2. divide auto-precharge process read/write
 *			3. divide precharge_flag_kill process normal/read_autopre/write_autopre
 *			4. generate wr_autoprecharge2 process excuting when tRDL=2
 *			5. modify same-bank-autoprecharge-disable function
 *			   (disable gapless, other bank interrupt case)
 * Revision No. : V3.1 
 * date         : May. 19. 1999  
 *			1. Add 16M 5th Gen. LVTTL(-10,-8,-8.7,-7,-6)
 *			2. Modify 2Mx32 5th Gen. tSAC value
 * Revision No. : V3.2 
 * date         : Jun. 11. 1999  
 *			1. Modify -A AC parameter(64M 5th, 128M 2th, 256M 2th)
 * Revision No. : V3.3 
 * date         : Jun. 24. 1999  
 *			1. After 64MSD3, auto_flagx signal is not used, 
 *			   let auto_flagx be not enabled to device which is after 64MSD3
 *			2. compile new versions 
 *			   verilog-xl : 2.7.17, NC-verilog : 2.1., VCS : 5.0.1a
 * Revision No. : V3.4 
 * date         : Jul. 12. 1999  
 *			1. To prevent roundoff error in AC parameter violation check procedure,
 *			   define MARGIN paramter. (MARGIN=0.1ns now)
 * Revision No. : V3.5 
 * date         : Sep. 28. 1999  
 *			1. Warning message 'tCCmax violation' is appeared during self refresh operation.
 *			   When self refresh or precharge-power down, clk can be don't care state. (can stay 'low' or 'high'-level)
 *                         Therefore tCCmax violation should not be checked during self refresh or prech. power down.
 * Revision No. : V3.6 
 * date         : Dec. 14. 1999  
 *			1. Make MODELSIM version and add partial REF./VREFi change function for NOKIA
 * Revision No. : V3.7 
 * date         : Jul. 11. 2000  
 *			1. Add dynamic memory allocation function using VERILOG-XL PLI,
 *			   then you must build verilog-XL excutable file containing PLI, and use +define+DYMEM option.
 * Revision No. : V3.8 
 * date         : Jul. 13. 2000  
 *			1. Add 128M SDRAM 3rd/4th Gen., 256M SDRAM 3rd Gen.
 * Revision No. : V3.9
 * date         : Feb. 5. 2001
 *                      1. remove CL2 parameter at -A, -7,... when violated, error messages are printed.
 * Revision No. : V4.0
 * date         : Jun. 11. 2001
 *                      1. 128M SDRAM X32 MCP (64M SDRAM X16 7th) parameter added
 * Revision No. : V4.1
 * date         : Jun. 23. 2001
 *                      1. 256M SDRAM 4th parameters are added
 * Revision No. : V4.2
 * date         : Jul. 31. 2001
 *                      1. because array of reg is not work at vcs 6.0, change array of reg to real variable (TRAS_PP, TPRE_P)
 * Revision No. : V4.3
 * date         : Aug. 09. 2001
 *                      1. There is an error that use "<" instead of ">", in tRASmax violation check routine.
 * Revision No. : V4.4
 * date         : Oct. 05. 2001
 *                      1. Add 128M Mobile SDRAM 5th Gen. (8Mx16, D-die)
 *                      2. Add 64M Mobile SDRAM 7th Gen. (2Mx32, F-die)
 * Revision No. : V4.5
 * date         : Nov. 23. 2001
 *                      1. Add mobile function (EMRS:PASR/TCSR/DSC, DPD:pin/cmd control)
 * Revision No. : V4.6
 * date         : Jan. 15. 2002
 *                      1. Add 512M SDRAM
 * Revision No. : V4.6
 * date         : Aug. 27. 2002
 *                      1. Add 128M X32 SDRAM
 * Revision No. : V4.7
 * date         : Nov. 25. 2003
 *                      1. Add 256M X32 SDRAM 
 *                      2. Add 512M X32 SDRAM (DDP)
 *                      3. Add 512M X16 SDRAM (DDP: 2CS)
 *                      4. Add 1GM X4 SDRAM (DDP: 2CS)
 *                      5. Add 1GM X8 SDRAM (DDP: 2CS, 2CKE)
 *==================================================================
 */

`timescale	1ns / 10ps
`default_nettype none

`define K1	1024
`define M1	1048576
`define BYTE	8


`ifdef CS2
    `ifdef DPD_PIN
        module sdram_cs2(clk, csb, cke, ba, ad, rasb, casb, web, dqm, dqi, dpdb);
    `else
        module sdram_cs2(clk, csb, cke, ba, ad, rasb, casb, web, dqm, dqi);
    `endif
`else
    `ifdef DPD_PIN
        module sdram(clk, csb, cke, ba, ad, rasb, casb, web, dqm, dqi, dpdb);
    `else
        module sdram(clk, csb, cke, ba, ad, rasb, casb, web, dqm, dqi);	//default port name
    `endif
`endif

    `ifdef M16
	`define TBITS	16*`M1
	`define nBank	2
        `define ADDRTOP	10
	`define	ADDR_AP	10
        `ifdef G2
                `define M16G2_M641G
	`else
                `define M16G3_M64G2
                `define M64G3_M128_M256
        `endif
    `endif

    `ifdef M64
	`define TBITS		64*`M1
	`define	ADDR_AP	10
    	`ifdef NBANK2
      	    `define nBank	2	
	    `ifdef X32
	    	`define ADDRTOP	11
	    `else
            	`define ADDRTOP	12
	    `endif
    	`endif
        `ifdef NBANK4
     	    `define nBank	4	
	    `ifdef X32
            	`define ADDRTOP	10
	    `else
	    	`define ADDRTOP	11
	    `endif
    	`endif
                `define M64_M128_M256    // for prech_reg 
        `ifdef G1
                `define M16G2_M641G
        `else
        `ifdef G2
                `define M16G3_M64G2
        `else
                `define M16G4_M64G3
                `define M64G3_M128_M256
        `endif
        `endif
    `endif

    `ifdef M128
	`define TBITS	128*`M1
        `ifdef NBANK4
     	    `define nBank	4	
	    `define ADDRTOP	11
    	`endif
	`define	ADDR_AP	10
        `define M64_M128_M256    // for prech_reg 
	`define M128_M256        // RFU, c_addr 10->11 
        `define M64G3_M128_M256  // from M64G3
        `define M16G4_M64G3
    `endif
    `ifdef M256
	`define TBITS	256*`M1
	`define nBank	4
	    `ifdef X32
        	`define ADDRTOP	11
	    `else
	    	`define ADDRTOP	12
	    `endif
	`define	ADDR_AP	10
        `define M64_M128_M256    // for prech_reg 
	`define M128_M256        // RFU, c_addr 10->11 
        `define M64G3_M128_M256  // from M64G3
        `define M16G4_M64G3
    `endif

    `ifdef M512
	`define TBITS	512*`M1
	`define nBank	4
        `define ADDRTOP	12
	`define	ADDR_AP	10
        `define M64_M128_M256    // for prech_reg 
	//`define M128_M256        // RFU, c_addr 10->11 
        `define M64G3_M128_M256  // from M64G3
        `define M16G4_M64G3
    `endif

    `ifdef M1024
	`define TBITS	1024*`M1
	`define nBank	4
        `define ADDRTOP	12
	`define	ADDR_AP	10
        `define M64_M128_M256    // for prech_reg 
	//`define M128_M256        // RFU, c_addr 10->11 
        `define M64G3_M128_M256  // from M64G3
        `define M16G4_M64G3
    `endif

`ifdef M1024 
    `ifdef X4
        `define M512_M1024_X4
        `define B               4
        `define nCOL            12
        `define PAGEDEPTH       4096
        `define nDQM            1
    `endif
    `ifdef X8
        `define M512_M1024_X8
        `define B               8
        `define nCOL            11
        `define PAGEDEPTH       2048
        `define nDQM            1
    `endif
`else

`ifdef M512
    `ifdef X4
	`define M512_M1024_X4
        `define B		4
        `define nCOL		12
        `define	PAGEDEPTH	4096
        `define nDQM		1
    `endif
    `ifdef X8
	`define M512_M1024_X8
        `define B		8
        `define nCOL		11
        `define	PAGEDEPTH	2048
        `define nDQM		1
    `endif
    `ifdef X16
        `define B		16
	    `ifdef CS2
        	`define nCOL	9
        	`define	PAGEDEPTH	512
	    `else
	    	`define nCOL	10
        	`define	PAGEDEPTH	1024
	    `endif
        `define nDQM		2
    `endif
    `ifdef X32
        `define B		32
        `define nCOL		9
        `define	PAGEDEPTH	512
        `define nDQM		4
    `endif
`else
`ifdef M256
    `ifdef X4
    	`define M128_M256_X4			// 98.6.30 BYC
    	`define B		4		// number of bit(x4)
    	`define nCOL		11
    	`define PAGEDEPTH	2048
	`define nDQM		1
    `endif
    `ifdef X8
        `define B		8		// number of bit(x8)
    	`define nCOL		10
    	`define PAGEDEPTH	1024
    	`define nDQM		1
    `endif
    `ifdef X16
    	`define B		16		// number of bit(x16)
    	`define nCOL		9
    	`define PAGEDEPTH	512
	`define nDQM		2
    `endif
    `ifdef X32
    	`define B		32		// number of bit(x32)
    	`define nCOL		9
    	`define PAGEDEPTH	512
	`define nDQM		4
    `endif
`else
`ifdef M128
    `ifdef X4
        `define M128_M256_X4                    // 98.6.30 BYC
        `define B               4               // number of bit(x4)
        `define nCOL            11
        `define PAGEDEPTH       2048
        `define nDQM            1
    `endif
    `ifdef X8
        `define B               8               // number of bit(x8)
        `define nCOL            10
        `define PAGEDEPTH       1024
        `define nDQM            1
    `endif
    `ifdef X16
        `define B               16              // number of bit(x16)
        `define nCOL            9
        `define PAGEDEPTH       512
        `define nDQM            2
    `endif
    `ifdef X32
        `define B               32              // number of bit(x32)
        `define nCOL            8
        `define PAGEDEPTH       256
        `define nDQM            4
    `endif
`else	// 64M & 16M
    `ifdef X4
    	`define B		4		// number of bit(x4)
    	`define nCOL		10
    	`define PAGEDEPTH	1024
	`define nDQM		1
    `endif
    `ifdef X8
        `define B		8		// number of bit(x8)
    	`define nCOL		9
    	`define PAGEDEPTH	512
    	`define nDQM		1
    `endif
    `ifdef X16
    	`define B		16		// number of bit(x16)
    	`define nCOL		8
    	`define PAGEDEPTH	256
	`define nDQM		2
    `endif
    `ifdef X32
    	`define B		32		// number of bit(x32)
    	`define nCOL		8
    	`define PAGEDEPTH	256
	`define nDQM		4
    `endif
`endif
`endif
`endif
`endif

`ifdef tCC1
`else
	`define NO_CL1
`endif
`ifdef tSAC1
`else
	`define NO_CL1
`endif
`ifdef tSHZ1
`else
	`define NO_CL1
`endif
`ifdef tCC2
`else
	`define NO_CL2
`endif
`ifdef tSAC2
`else
	`define NO_CL2
`endif
`ifdef tSHZ2
`else
	`define NO_CL2
`endif
`ifdef tCC3
`else
	`define NO_CL3
`endif
`ifdef tSAC3
`else
	`define NO_CL3
`endif
`ifdef tSHZ3
`else
	`define NO_CL3
`endif

`ifdef	tRDL
`else
	`define	tRDL	1
`endif

`ifdef DPD_CMD
	`define DPD
`endif
`ifdef DPD_PIN
	`define DPD
`endif

  `define HB		`B/2
  `define BIT		`B-1:0
  `define BIT_C		`nCOL-1:0
  `define BIT_T		`nCOL+`ADDRTOP:0
  `define nWORD		`TBITS/`B/`nBank
  `define WORD		`nWORD-1:0
  
  inout   [`BIT]  dqi;
  input   [`nBank/2-1:0] ba;
  input   [`ADDRTOP:0]  ad;
  input   rasb,casb,web;
  input   clk,cke,csb;
  input   [`nDQM-1:0] dqm;
`ifdef DPD_PIN
  input   dpdb;
`endif
  
/*
  `ifdef M64
    	`include "m64.ac"
  `endif
  `ifdef M16
    	`include "m16.ac"
  `endif
  `ifdef M128
    	`include "m128.ac"
  `endif
  `ifdef M256
    	`include "m256.ac"
  `endif
*/

  parameter       pwrup_time = 200000, pwrup_check = 1;

wire     [`nBank/2 + `ADDRTOP : 0] addr;
assign addr = {ba, ad};

wire	[`BIT]	dqi;
`ifdef DYMEM
initial begin
	$damem_declare("mem_a", `B-1, 0, `nWORD-1, 0);
	$damem_declare("mem_b", `B-1, 0, `nWORD-1, 0);
end
`else
reg		[`BIT]	mem_a[`WORD];	// memory cell array of a bank
reg		[`BIT]	mem_b[`WORD];	// memory cell array of b bank
`endif
`ifdef NBANK4
`ifdef DYMEM
initial begin
	$damem_declare("mem_c", `B-1, 0, `nWORD-1, 0);
	$damem_declare("mem_d", `B-1, 0, `nWORD-1, 0);
end
`else
reg		[`BIT]	mem_c[`WORD];	// memory cell array of c bank
reg		[`BIT]	mem_d[`WORD];	// memory cell array of d bank
`endif
`endif

reg 	[`BIT] 	dqo, t_dqo;	// output temp. register declaration
reg		[`ADDRTOP:0]	r_addr_[`nBank-1:0];
reg		[`ADDRTOP:0]	r_addr;
reg		[`BIT_C] c_addr;	// column address
reg		[`BIT_T] m_addr; 	// merge row and column address 
reg 	[`BIT]  dout_reg[`PAGEDEPTH:0];
reg 	[`BIT]  din_rega[`PAGEDEPTH:0];	// din register for a bank
reg 	[`BIT]  din_regb[`PAGEDEPTH:0];	// din register for b bank
`ifdef NBANK4
reg 	[`BIT]  din_regc[`PAGEDEPTH:0];	// din register for c bank
reg 	[`BIT]  din_regd[`PAGEDEPTH:0];	// din register for d bank
`endif
reg 	[`BIT]  clk_dq;				
reg		ptr;

reg		[`BIT]	ZDATA;
reg		[7:0] ZBYTE;


// define mode dependency flag
`define INITIAL 0	// no bank precharge
/*
`define IDLE_AB	1	// both bank precharge
`define ACT_A   2	// a bank active and b bank precharge
`define ACT_B   3	// b bank active and a bank precharge
`define ACT_AB  4	// a & b bank active
`define IDLE_A  5   // only a bank prechage
`define IDLE_B  6   // only b bank prechage
*/

`define TRUE   1
`define FALSE  0
`define HIGH   1
`define LOW    0
`define	MARGIN	0.1

//parameter	pwrup_time = 200000, pwrup_check = 1;

/*
 *-----------------------------------------------------
 *	We know the phase of external signal 
 *	by examining the state of its flag.
 *-----------------------------------------------------
 */

reg		r_bank_addr;				// row bank check flag	
reg		[`nBank/2-1:0] c_bank_addr;				// column bank check flag	
reg		[`nBank-1:0] auto_flag;					// auto precharge flag
reg		burst_type,					// burst type flag
		auto_flagx,
		self_flag;					// auto & self refresh flag
integer	kill_bank, wr_kill_bank, rd_kill_bank;
integer	k;
reg		[`nBank-1:0] precharge_flag;			// precharge each bank check flag
reg		[`nBank/2:0] prech_reg;		// precharge mode (addr[13:12] && addr[10])
reg		[`nBank/2-1:0]	rd_autoprech_reg;
reg		[`nBank/2-1:0]	wr_autoprech_reg;
reg		[`nBank/2-1:0]	wr_autoprech_reg2;
reg		[`nBank/2-1:0]	prev_ba;		// bank address of previous command
reg		pwrup_done;
reg		[`nBank-1 : 0]	first_pre;
//reg		[8*8 : 1]	str;
integer auto_cnt;
integer i;

`ifdef M16
	wire	[3:0]	RFU = {addr[11:10], addr[8], addr[7]}; 
`endif

`ifdef M64
  `ifdef NBANK2
	wire	[3:0]	RFU = {addr[11:10], addr[8], addr[7]}; 
  `endif
  `ifdef NBANK4
	`ifdef X32
	  wire	[4:0]	RFU = {addr[12:10], addr[8], addr[7]}; 
	`else
	  wire	[5:0]	RFU = {addr[13:10], addr[8], addr[7]}; 
	`endif
  `endif
`endif

`ifdef M128 
  `ifdef NBANK4
	  wire	[5:0]	RFU = {addr[13:10], addr[8], addr[7]}; 
  `endif
`endif

`ifdef M256 
	`ifdef X32
	  wire	[5:0]	RFU = {addr[13:10], addr[8], addr[7]}; 
	`else
	  wire	[6:0]	RFU = {addr[14:10], addr[8], addr[7]}; 
	`endif
`endif

`ifdef M512 // 98.6.30 BYC
	wire	[6:0]	RFU = {addr[14:10], addr[8], addr[7]}; 
`endif

`ifdef M1024 
	wire	[6:0]	RFU = {addr[14:10], addr[8], addr[7]}; 
`endif

reg [`nBank-1:0] Mode;			// check mode dependency
reg 	[`nBank-1:0] md;
reg		rd_reautoprecharge, wr_reautoprecharge;
`ifdef NOKIA
	reg	REF16M_MODE, REF32M_MODE;
`endif
`ifdef DPD
	`define	tDPDEXIT	200000
	reg	D_POWERDOWN, D_PDOWN_EXIT;
	integer	PROC_DPDEXIT;
`endif
`ifdef MOBILE
	reg	REF4BANK, REF2BANK, REF1BANK;
`endif
integer	BL, WBL, CL;	// burst length & cas latency
real 	tSHZ;			// clk to output in hi-Z
real	tSAC;			// clk to valid output
reg	write_event;//KyW ... 0408 for VCS

event
	active,		// main operation of SDRAM
	modeset,
	read,
	dqo_event,
	write,
	flush_write,
	precharge,
	rd_autoprecharge,
	wr_autoprecharge,
	wr_autoprecharge2,
	precharge_start,
	precharge_flag_kill,
	wr_precharge_flag_kill,
	rd_precharge_flag_kill,
	autorefresh,
	autostart,
	selfrefresh,
`ifdef DPD
	deeppowerdown,
	d_pdown_exit,
`endif
	selfexit;

`protect

// initialize each flag
initial	
	begin
		for (i = 0; i < `nBank; i = i + 1)
		  auto_flag[i]  = `FALSE;
		auto_flagx = `FALSE;
		rd_reautoprecharge =`FALSE;
		wr_reautoprecharge =`FALSE;
		self_flag  = `FALSE;
		pwrup_done = `FALSE;
		Mode = `nBank'b0;

		for(i = 0; i < `nBank; i = i + 1)
		begin
			first_pre[i]  = `TRUE;
			precharge_flag[i] = `FALSE;
		end

		ZBYTE = 8'bz;
	    for (i = 0; i < `B; i = i + 1) begin
			ZDATA[i] = 1'bz;
		end
	end

//--------------------------------------------------------------
//---------    TIMING VIOLATION CHECK ROUTINE
//--------------------------------------------------------------

real  CUR_TIME, TCKE, TADDR, TRASB, TCASB, TCSB, TWEB, TDQI, TCLK_H, TCLK_L, 
	  TCC_P, pclk_high, last_read, last_rw;

reg [63:0] TDQM[`nDQM-1:0];

//real  TRAS_P, TCAS_P, TRASA_P, TRASB_P, TPREA_P, TPREB_P, TSELF, TSEXIT;
// 4 bank
real  TRAS_P, TCAS_P, TSELF, TSEXIT;
//reg  [63:0] TRAS_PP [`nBank-1:0];	
//reg  [63:0] TPRE_P  [`nBank-1:0];
real	TRAS_PP0, TRAS_PP1, TRAS_PP2, TRAS_PP3;
real	TPRE_P0, TPRE_P1, TPRE_P2, TPRE_P3;

reg   CKE_FLAG, CSB_FLAG, RASB_FLAG, CASB_FLAG, WEB_FLAG;

//event MRS, ACTIVE;
reg   MRS_SET, WRITE_MODE, READ_MODE, UNMODE, POWERDOWN_MODE, POWERDOWN_MODE1,//KyW ... 0928 for NOKIA claim
	  SUSPEND_MODE, AUTOREF_MODE, SELFREF_MODE;
reg   PWR, INIT;


`define NOP   (RASB_FLAG == `HIGH && CASB_FLAG == `HIGH && WEB_FLAG == `HIGH)
`define NOP1  (RASB_FLAG == `HIGH && CASB_FLAG == `HIGH)
/*
 *-----------------------------------------------------
 *	 wire declaration 
 *-----------------------------------------------------
 */

reg      pcke;
reg [`nDQM-1:0] dqm_r;
reg [`nDQM-1:0] dqm_ri;

reg      data_read;
reg      tdata_read;
reg [`BIT] clkh_dq;
reg [2:0]  prev_com;
reg          rw_dqm;
reg        gapless;
wire     pclk = pcke & clk;
wire [2:0] com = {RASB_FLAG, CASB_FLAG, WEB_FLAG};
wire #(TCC_P+0.02) data_read_delay = data_read; // 98.6.29 BYC
`endprotect

`ifdef X32
assign   #(tSAC, tSAC, tSHZ) dqi[`B-1:`B-8]= ( data_read & ~dqm_r[3] & ~rw_dqm)?dqo[`B-1:`B-8]:ZBYTE;
assign   #(tSAC, tSAC, tSHZ) dqi[`B-9:`B-16] = ( data_read & ~dqm_r[2] & ~rw_dqm)?dqo[`B-9:`B-16]:ZBYTE;
assign   #(tSAC, tSAC, tSHZ) dqi[`B-17:`B-24]= ( data_read & ~dqm_r[1] & ~rw_dqm)?dqo[`B-17:`B-24]:ZBYTE;
assign   #(tSAC, tSAC, tSHZ) dqi[`B-25:0] = ( data_read & ~dqm_r[0] & ~rw_dqm)?dqo[`B-25:0]:ZBYTE;
`endif
`ifdef X16
assign   #(tSAC, tSAC, tSHZ) dqi[`B-1:`B-8]= ( data_read & ~dqm_r[1] & ~rw_dqm)?dqo[`B-1:`B-8]:ZBYTE;
assign   #(tSAC, tSAC, tSHZ) dqi[`HB-1:0] = ( data_read & ~dqm_r[0] & ~rw_dqm)?dqo[`B-9:0]:ZBYTE;
`endif
`ifdef X8
assign   #(tSAC, tSAC, tSHZ) dqi[`B-1:0] = ( data_read & ~dqm_r & ~rw_dqm)?dqo[`B-1:0]:ZDATA;
`endif
`ifdef X4
assign   #(tSAC, tSAC, tSHZ) dqi[`B-1:0] = ( data_read & ~dqm_r & ~rw_dqm)?dqo[`B-1:0]:ZDATA;
`endif

`protect
always @(posedge pclk) begin
    pclk_high <= #0.01 $realtime;
    clkh_dq <= #0.01 dqi;
end


always @(READ_MODE) begin
//	data_read <= repeat(CL-1) @(posedge pclk) READ_MODE;
//  VCS does not support above statement. However Verilog-XL is OK. 
//  So, I modified as following statement for VCS.
	#0.1;
	if (READ_MODE == 1'b1)
		data_read <= repeat(CL-1) @(posedge pclk) 1'b1;
	else
		data_read <= repeat(CL-1) @(posedge pclk) 1'b0;
end

always @(negedge tdata_read) begin
	data_read = tdata_read;
	tdata_read = `TRUE;
end

always @(dqo_event)
	dqo <= repeat(CL-1) @(posedge pclk) t_dqo;

/*
 *-----------------------------------------------------
 * setup hold check	
 *-----------------------------------------------------
 */

initial #0.01 pcke = cke;

initial  // time variables initialization
	begin
		$timeformat(-9, 1, " ns", 10);
		TCKE = 0;
		TADDR = 0;
		TRASB = 0;
		TCASB = 0;
		TCSB = 0;
		TWEB = 0;
		TDQI = 0;
		TCLK_H = -20;
		TCLK_L = -20;
		TRAS_P = -200;
		TCAS_P = -200;
		TSELF   = -200;
        pclk_high = -20;
        last_read = -200;
        last_rw = -20;
		for (i = 0; i < `nDQM; i = i + 1)
		  TDQM[i] = 64'b0;
// 4bank
		//TRAS_PP[0] = -200;
		//TRAS_PP[1] = -200;
		//TPRE_P[0] = -200;
		//TPRE_P[1] = -200;
		TRAS_PP0 = -200;
		TRAS_PP1 = -200;
		TPRE_P0 = -200;
		TPRE_P1 = -200;
	`ifdef NBANK4
		//TRAS_PP[2] = -200;
		//TRAS_PP[3] = -200;
		//TPRE_P[2] = -200;
		//TPRE_P[3] = -200;
		TRAS_PP2 = -200;
		TRAS_PP3 = -200;
		TPRE_P2 = -200;
		TPRE_P3 = -200;
	`endif

	end

initial  // mode register variables initialization
	begin
		RASB_FLAG = `HIGH;
		CASB_FLAG = `HIGH;
		CSB_FLAG  = `HIGH;
		WEB_FLAG  = `HIGH;
	end

initial  // mode register variables initialization
	begin
		INIT        = `TRUE;
		MRS_SET     = `FALSE;
		WRITE_MODE  = `FALSE;
		READ_MODE   = `FALSE;
		POWERDOWN_MODE = `FALSE;
		POWERDOWN_MODE1 = `FALSE;//KyW ... 0928 for NOKIA claim
		SUSPEND_MODE = `FALSE;
		AUTOREF_MODE   = `FALSE;
		SELFREF_MODE   = `FALSE;
`ifdef NOKIA
		REF16M_MODE = `FALSE;
		REF32M_MODE = `FALSE;
`endif
`ifdef MOBILE
		REF4BANK = `TRUE;
		REF2BANK = `FALSE;
		REF1BANK = `FALSE;
`endif
`ifdef DPD
		D_POWERDOWN = `FALSE;
		D_PDOWN_EXIT = `FALSE;
		PROC_DPDEXIT = 0;
`endif
		write_event = `FALSE; //KyW ... 0408 for VCS
	end

always @(POWERDOWN_MODE) POWERDOWN_MODE1 = #0.1 POWERDOWN_MODE;//KyW ... 0928 for NOKIA claim

always @( posedge clk )
	if( PWR == `TRUE )
	begin : main
		CUR_TIME = $realtime;

		if( POWERDOWN_MODE == `TRUE && CKE_FLAG == `TRUE )
		begin
			if( SELFREF_MODE == `TRUE )
			  begin
				if( CUR_TIME - TSELF < `tRASmin-`MARGIN )
				begin
				  $display("Warning: tRAS violation in self refresh at %t", CUR_TIME);
				end
				->selfexit;
`ifdef v			$display(">> self refresh exit at 	%t", CUR_TIME);
`endif
`ifdef v //			$display(">> power down exit at 	%t", CUR_TIME);
`endif
				POWERDOWN_MODE = `FALSE;
				CKE_FLAG = `FALSE;
`endprotect
				`ifdef M16G2_M641G
				  pcke <= repeat (1) @(negedge pclk)  cke;
				`else
				  #0 pcke = cke;
				`endif
				
			end
`ifdef DPD
			else if (D_POWERDOWN == `TRUE)
			begin
				->d_pdown_exit;
`ifdef v			$display(">> deep power down exit at 	%t", CUR_TIME);
`endif
				POWERDOWN_MODE = `FALSE;
				CKE_FLAG = `FALSE;
			end
`endif
			else
			  begin
				`ifdef M16G2_M641G
				  if (CUR_TIME - TCKE >= `tPDE-`MARGIN)
				`else
				  if( CUR_TIME - TCKE >= `tSS-`MARGIN )
				`endif
				begin
`ifdef v				$display(">> power down exit at 	%t", CUR_TIME);
`endif
					POWERDOWN_MODE = `FALSE;
					CKE_FLAG = `FALSE;
				    pcke <= repeat (1) @(negedge pclk)  cke; 
				end
				else
				begin
					`ifdef M16G2_M641G
					  $display("Warning: tPDE violation at %t", CUR_TIME);
					`else
					  $display("Warning: tSS Precharge Power Down Exit Setup Violation at %t",CUR_TIME);
					`endif
					disable main;
				end
			end
		end

		if( POWERDOWN_MODE == `FALSE)
		begin
			if( CUR_TIME - TCKE < `tSS-`MARGIN )	// check cke setup timing
				$display("Warning: CKE setup violation at %t", CUR_TIME);
			else if( cke && SUSPEND_MODE == `TRUE )
			begin
`ifdef v			$display(">> clock suspension exit at 	%t", CUR_TIME);
`endif
				SUSPEND_MODE = `FALSE;
				pcke <= @(negedge clk) cke;
			end
			else
				pcke <= @(negedge clk) cke;
		end

// clock timing check

			TCC_P = CUR_TIME - TCLK_H;  // saving current clock period

			if( CUR_TIME - TCLK_H < `tCCmin-`MARGIN && POWERDOWN_MODE1 == `FALSE)//KyW ... 0928 for NOKIA claim
				$display("Warning: tCCmin violation at %t", CUR_TIME);

			if( CUR_TIME - TCLK_H > `tCCmax+`MARGIN && POWERDOWN_MODE1 == `FALSE)//KyW ... 0928 for NOKIA claim
				$display("Warning: tCCmax violation at %t", CUR_TIME);
`protect
			if( CUR_TIME - TCLK_L < `tCL-`MARGIN )
				$display("Warning: tCL violation at %t", CUR_TIME);

// pcke is high
		if( pcke ) begin

// csb timing check
			if( CUR_TIME - TCSB < `tSS-`MARGIN )
				$display("Warning: CSB setup violation at %t", CUR_TIME);
			else  //  if( CUR_TIME - TCSB < `tCCmin + `tSS-`MARGIN )
				CSB_FLAG = csb;

// if chip selected
			if( CSB_FLAG == `LOW )
			begin
				if( CUR_TIME - TRASB < `tSS-`MARGIN )
					$display("Warning: RASB setup violation at %t", CUR_TIME);
				else 
					RASB_FLAG = rasb;

				if( CUR_TIME - TCASB < `tSS-`MARGIN )
					$display("Warning: CASB setup violation at %t", CUR_TIME);
				else 
					CASB_FLAG = casb;
					
				if( CUR_TIME - TWEB < `tSS-`MARGIN )
					$display("Warning: WEB setup violation at %t", CUR_TIME);
				else 
					WEB_FLAG = web;

			end
			for (i = 0; i < `nDQM; i = i + 1) begin
			  if( CUR_TIME - $bitstoreal(TDQM[i]) < `tSS-`MARGIN )
				$display("Warning: DQM(%d) setup violation at %t", i, CUR_TIME);
			  else begin
//				dqm_r[i] <=  @(posedge pclk) dqm[i];
//  VCS does not support above statement. However Verilog-XL is OK. 
//  So, I modified as following statement for VCS.
				dqm_r[i] <= dqm_ri[i];
				dqm_ri[i] <= dqm[i];
			  end
			end
		end

		TCLK_H = CUR_TIME; // the time clock is high

	end

/*------------------------------------------
 *       command recognition
 *------------------------------------------
 */

always @( posedge pclk )
	if( PWR == `TRUE )
	begin : command
		integer bank_id;
		reg		[8*8:1] str;

		if( CSB_FLAG == `LOW )
		begin : command_sel
			if( auto_cnt == -1 )
			begin
				if( ~ `NOP ) // NOP1 -> NOP because of burst stop
					$display("Warning: NOP required during power-up pause time.");
				disable command_sel;
			end

			if( CUR_TIME - TADDR < `tSS-`MARGIN )
				$display("Warning: Address setup violation at %t", CUR_TIME);

			if( `NOP ) 	// deleted by burst stop -> NOP1 -> NOP
				disable command_sel;

			if( AUTOREF_MODE == `TRUE || SELFREF_MODE == `TRUE )
			begin
				$display("Warning: Illegal command in refresh operation at %t", CUR_TIME);
				disable command_sel;
			end
`ifdef DPD
			if (D_POWERDOWN == `TRUE) begin
				$display("Warning: Illegal command in deep power down exit at %t", CUR_TIME);
				disable command_sel;
			end
			if (D_PDOWN_EXIT) begin
				if (PROC_DPDEXIT == 0) begin
					if ({RASB_FLAG,CASB_FLAG,WEB_FLAG} == 3'b010) begin
					`ifdef M64_M128_M256
						`ifdef NBANK2
						  prech_reg[1] = ba;
						  prech_reg[0] = addr[`ADDR_AP];
						`endif
						`ifdef NBANK4
						  prech_reg[2] = ba[1];
						  prech_reg[1] = ba[0];
						  prech_reg[0] = addr[`ADDR_AP];
						`endif
					`endif
						PROC_DPDEXIT = 1;
						->precharge;
					end
					else $display("Warning: all bank precharge required to complete deep power down mode exit.");
				end
				else if (PROC_DPDEXIT == 1 || PROC_DPDEXIT == 2) begin
					if ({cke,RASB_FLAG,CASB_FLAG,WEB_FLAG} == 4'b1001) begin
						PROC_DPDEXIT = PROC_DPDEXIT + 1;
						->autorefresh;
					end
					else $display("Warning: 2 or more auto refresh required to complete deep power down mode exit.");
				end
				else if (PROC_DPDEXIT == 3) begin
					if ({RASB_FLAG,CASB_FLAG,WEB_FLAG} == 3'b000) begin
						D_PDOWN_EXIT = `FALSE;
						PROC_DPDEXIT = 0;
						->modeset;
					end
					else if ({cke,RASB_FLAG,CASB_FLAG,WEB_FLAG} == 4'b1001) begin
						->autorefresh;
					end
					else $display("Warning: Mode register set required to complete deep power down mode exit.");
				end
		end
						
`endif
			if( ~pwrup_done )
				if( auto_cnt < 2 )	
				begin
					case ( {RASB_FLAG,CASB_FLAG,WEB_FLAG} )
						'b010 :
							begin
							  `ifdef M64_M128_M256
								`ifdef NBANK2
								  prech_reg[1] = ba;
								  prech_reg[0] = addr[`ADDR_AP];
								`endif
								`ifdef NBANK4
								  prech_reg[2] = ba[1];
								  prech_reg[1] = ba[0];
								  prech_reg[0] = addr[`ADDR_AP];
								`endif
							  `endif
							  `ifdef M16
								prech_reg = addr[11:10];
							  `endif
								->precharge;
							end
						'b001 :
							if( cke )
								begin
									auto_cnt = auto_cnt + 1; 
									->autorefresh;
								end
						'b000 :
							begin
                    			if( MRS_SET == `TRUE )
                        			->modeset;
                    			else if( INIT == `TRUE )
                    			begin
                        			->modeset;
                        			INIT = `FALSE;
                    			end
                    			else
                        			$display("Warning: MODE register set need 2 clock cycles at %t", CUR_TIME);
                			end
							
						default: begin
							$display("Warning: 2 or more auto-refresh required during power up sequence.");  
						end
					endcase
					disable command_sel;
				end
				else 
					pwrup_done = `TRUE;

			case( {RASB_FLAG,CASB_FLAG,WEB_FLAG} )
			'b000 :
				begin
					if( MRS_SET == `TRUE )
						->modeset;
					else if( INIT == `TRUE )
					begin
						->modeset;
						INIT = `FALSE;
					end
					else
						$display("Warning: MODE register set need 2 clock cycles at %t", CUR_TIME);
				end
			'b011 :		// active
				begin
					if( MRS_SET == `TRUE )
					begin
						bank_id = BankSelect(ba);
						str = PrintBank(ba);
						if(Mode[bank_id] == `TRUE)
							$display("Warning: Illegal active (%0s is not precharged) at %t",str,CUR_TIME);

						else
							->active;
					end

					else if( INIT == `FALSE )
						$display("Warning: MODE register set need 2 clock cycles at %t",CUR_TIME);
					else if( INIT == `TRUE )
						$display("Warning: Initial MODE register set must be required before active at %t",CUR_TIME);
				end
`endprotect
			'b101 :
				begin	// read command
					bank_id = BankSelect(ba);
					str = PrintBank(ba);
                    gapless = (pclk_high == last_rw && auto_flag[bank_id]) ? 
								`TRUE : `FALSE;
					`ifdef M16G4_M64G3
					  if (precharge_flag[bank_id])
					    begin
						  $display("Warning: Illegal command in precharge operation at %t",CUR_TIME);
						  disable command_sel;
					    end
					`else
					  if( auto_flagx )
					    begin
						  $display("Warning: Illegal command in auto-precharge command at %t",CUR_TIME);
						  disable command_sel;
					    end
					`endif
					else if( Mode[bank_id] == `TRUE)
					begin
						if( READ_MODE == `TRUE)
						begin
`ifdef M64G3_M128_M256
							if (auto_flag[prev_ba] == `TRUE && prev_ba != bank_id) begin
								rd_autoprech_reg = prev_ba;
								->rd_autoprecharge;
							end
`endif
							READ_MODE = `FALSE;
							#0 disable read_block;
						end
						if( WRITE_MODE == `TRUE)
						begin
`ifdef M64G3_M128_M256
							if (auto_flag[prev_ba] == `TRUE && prev_ba != bank_id) begin
//`ifdef RDL2
								if (`tRDL == 2) begin
									wr_autoprech_reg2 = prev_ba;
									->wr_autoprecharge2;
								end
//`else
								else begin
									wr_autoprech_reg = prev_ba;
									->wr_autoprecharge;
								end
//`endif
							end
`endif
							WRITE_MODE = `FALSE;
							disable write_block;
						end
                        last_read = CUR_TIME;
                        prev_com = {RASB_FLAG,CASB_FLAG,WEB_FLAG};
			prev_ba = ba;
                        rw_dqm <= `FALSE;
						#0 ->read;
					end

					else
						$display("Warning: Illegal read (%0s is not active) at %t",str,CUR_TIME);
				end
			'b100 :
				begin	// write command
					bank_id = BankSelect(ba);
					str = PrintBank(ba);
                      if (prev_com == 3'b101) begin
                        if (clkh_dq !== `B'bz && data_read_delay == 1'b1) // 98.6.29 BYC
                            $display("Warning: HIZ should be issued 1 cycle before write op at %t", CUR_TIME);
                      end
					`ifdef M16G2_M641G
					`else
                      gapless = (pclk_high == last_rw && auto_flag[bank_id]) 
								? `TRUE : `FALSE;
					`endif
					`ifdef M16G4_M64G3
					  if (precharge_flag[bank_id])
					    begin
						  $display("Warning: Illegal command in precharge operation at %t",CUR_TIME);
						  disable command_sel;
					    end
					`else
					  if( auto_flagx )
					    begin
						  $display("Warning: Illegal command in auto-precharge command at %t",CUR_TIME);
						  disable command_sel;
					    end
					`endif
`protect
					else if( Mode[bank_id] == `TRUE)
					begin
						if( READ_MODE == `TRUE )
						begin
`ifdef M64G3_M128_M256
							if (auto_flag[prev_ba] == `TRUE && prev_ba != bank_id) begin
								rd_autoprech_reg = prev_ba;
								->rd_autoprecharge;
							end
`endif
							READ_MODE = `FALSE;
							tdata_read <= @(posedge pclk) READ_MODE;	// 3.2
							#0 disable read_block;
						end
						if( WRITE_MODE == `TRUE )
						begin
`ifdef M64G3_M128_M256
							if (auto_flag[prev_ba] == `TRUE && prev_ba != bank_id) begin
//`ifdef RDL2
								if (`tRDL == 2) begin
									wr_autoprech_reg2 = prev_ba;
									->wr_autoprecharge2;
								end
//`else
								else begin
									wr_autoprech_reg = prev_ba;
									->wr_autoprecharge;
								end
//`endif
							end
`endif
							WRITE_MODE = `FALSE;
							disable write_block;
						end
					        if( |dqm === 1'b0 ) begin // 98.6.26 BYC
						if( CUR_TIME - TDQI < `tSS-`MARGIN )
							$display("Warning: DQi setup violation at %t", CUR_TIME);
						end
                        prev_com = {RASB_FLAG,CASB_FLAG,WEB_FLAG};
			prev_ba = ba;
                        if (pclk_high == last_read)
                        rw_dqm <= @(negedge pclk) `TRUE;
						//#0 ->write;
						write_event <= `TRUE; //KyW ... 0408 for VCS
						write_event <= #0.1 `FALSE; //KyW ... 0408 for VCS
					end
					else 
						$display("Warning: Illegal write ( %0s is not active) at %t",str,CUR_TIME);
				
				end
			'b110 :  // burst stop
`ifdef DPD
				if( cke === 1'b0 )
					->deeppowerdown;
				else if (cke === 1'b1)
`endif
				begin
`ifdef v				$display ("Burst stop");
`endif


                    if( READ_MODE == `TRUE )
                    begin
                        READ_MODE = `FALSE;
`ifdef v                $display("-- reading burst stop at %t", CUR_TIME);
`endif
                        disable read_block;
                    end  
                    else if( WRITE_MODE == `TRUE)
                    begin
                         // at this clock, writing done
`ifdef v                 $display("-- writing burst stop at %t", CUR_TIME);
`endif
						`ifdef M16G2_M641G
                          WRITE_MODE =  @(negedge pclk) `FALSE;
						`else
                          WRITE_MODE =  `FALSE;
						`endif
                        disable write_block;
                    end  
                    prev_com = {RASB_FLAG,CASB_FLAG,WEB_FLAG};
                end
			'b010 :
				begin
`endprotect
				`ifdef M16G4_M64G3 //KyW ... 0624 : rev 3.3
				`else
					if(auto_flagx == `TRUE && (addr[`ADDR_AP] || ba == c_bank_addr))
					begin
						$display("Warning: Illegal command in auto-precharge command at %t",CUR_TIME);
						disable command_sel;
					end
				`endif
					`ifdef M64_M128_M256
					  `ifdef NBANK2
					    prech_reg[1] = ba;
					    prech_reg[0] = addr[`ADDR_AP];
					  `endif
					  `ifdef NBANK4
					    prech_reg[2] = ba[1];
					    prech_reg[1] = ba[0];
					    prech_reg[0] = addr[`ADDR_AP];
					  `endif
					`endif
					`ifdef M16
					  prech_reg = addr[11:10];
					`endif
					->precharge;
				end
`protect
			'b001 : begin
				if( cke )
					->autorefresh;
				else if( ~cke )
					->selfrefresh;
			end
//			default :
`ifdef v  //			$display("- NOP");
`endif
			endcase
		end  // command_sel

//		TCLK_H = CUR_TIME; // the time clock is high
	end

`ifdef DPD_PIN
always @(negedge dpdb)
	if (dpdb === 1'b0) ->deeppowerdown;
	else begin
`ifdef v	$display("Warning: Invalid dpdb pin state at %t", CUR_TIME);
`endif
	end
always @(posedge dpdb)
	if (dpdb === 1'b1) begin
		if (D_POWERDOWN == `TRUE)
			begin
				->d_pdown_exit;
`ifdef v			$display(">> deep power down exit at 	%t", CUR_TIME);
`endif
				POWERDOWN_MODE = `FALSE;
				CKE_FLAG = `FALSE;
			end
	end
	else begin
`ifdef v	$display("Warning: Invalid dpdb pin state at %t", CUR_TIME);
`endif
	end
`endif

// power down or supend enter mode
always @( posedge pclk )
	if( POWERDOWN_MODE == `FALSE && cke == `LOW && CUR_TIME - TCKE >= `tSS-`MARGIN )
	begin
		if( |Mode == 1'b0 ) //BYC
		begin
`ifdef v 		$display(">> power down enter at 	%t",CUR_TIME);
`endif
			POWERDOWN_MODE = `TRUE;
		end
		else
		begin
`ifdef v		$display(">> clock suspension enter at 	%t",CUR_TIME);
`endif
			SUSPEND_MODE = `TRUE;
		end
	end

// clock width check
always @( negedge clk )
	if( PWR == `TRUE )
	begin
		CUR_TIME = $realtime;
		if( CUR_TIME - TCLK_H < `tCH-`MARGIN )
			$display("Warning: tCH violation at %t", CUR_TIME);
		TCLK_L = CUR_TIME;
	end

// hold time check
always @(cke)
	begin
		CUR_TIME = $realtime;
		if( PWR == `TRUE )
			if( POWERDOWN_MODE == `FALSE && CUR_TIME - TCLK_H < `tSH-`MARGIN )
				$display("Warning: CKE hold violation at %t", CUR_TIME);
			else if( POWERDOWN_MODE == `TRUE && cke == `HIGH )
				CKE_FLAG = `TRUE;

		TCKE = CUR_TIME;
	end


always @(addr) //posedge addr or negedge addr )
	begin
		CUR_TIME = $realtime;
		if( PWR == `TRUE && CSB_FLAG == `LOW && pcke == `HIGH )
			if( CUR_TIME - TCLK_H < `tSH-`MARGIN )
				$display("Warning: Address hold violation at %t", CUR_TIME);
		TADDR = CUR_TIME;
	end

always @( negedge rasb or posedge rasb)
	begin
		CUR_TIME = $realtime;
		if( PWR == `TRUE && CSB_FLAG == `LOW )
			if( pcke == `HIGH && CUR_TIME - TCLK_H < `tSH-`MARGIN )
				$display("Warning: RASB hold violation at %t", CUR_TIME);
		if(rasb)
			RASB_FLAG = `HIGH;
		TRASB = CUR_TIME;
	end

always @( negedge casb or posedge casb)
	begin
		CUR_TIME = $realtime;
		if( PWR == `TRUE && CSB_FLAG == `LOW )
			if( pcke == `HIGH && CUR_TIME - TCLK_H < `tSH-`MARGIN )
				$display("Warning: CASB hold violation at %t", CUR_TIME);
		if(casb)
			CASB_FLAG = `HIGH;
		TCASB = CUR_TIME;
	end

always @( negedge csb or posedge csb)
	begin
		CUR_TIME = $realtime;
		if( csb )
			CSB_FLAG <= #(CUR_TIME - TCLK_H + `tSH) csb;
		if( PWR == `TRUE && pcke == `HIGH && CUR_TIME - TCLK_H < `tSH-`MARGIN )
			$display("Warning: CSB hold violation at %t", CUR_TIME);
		TCSB = CUR_TIME;
	end

always @( negedge web or posedge web)
	begin
		CUR_TIME = $realtime;
		if( PWR == `TRUE && CSB_FLAG == `LOW )
			if( pcke == `HIGH && CUR_TIME - TCLK_H < `tSH-`MARGIN )
				$display("Warning: WEB hold violation at %t", CUR_TIME);
		if(web)
			WEB_FLAG = `HIGH;
		else
			WEB_FLAG = `LOW;
		TWEB = CUR_TIME;
	end

always @(dqi)
	begin
		if( WRITE_MODE )
		begin
			CUR_TIME = $realtime;
			if( CUR_TIME - TCLK_H < `tSH-`MARGIN )
				$display("Warning: DQi hold violation at %t", CUR_TIME);
		end
		TDQI = $realtime;
	end

always @(dqm) begin
  CUR_TIME = $realtime;
  if (PWR == `TRUE)
	for (i = 0; i < `nDQM; i = i + 1) begin
	  if (CUR_TIME - TCLK_H < `tSH-`MARGIN && pcke == `HIGH)
		$display("Warning: DQM(%d) hold violation at %t", i, CUR_TIME);
	  TDQM[i] = $realtobits(CUR_TIME);
	end
end


/*
 *-----------------------------------------------------
 *	 power up check routine							
 *-----------------------------------------------------
 */

initial
	begin
		auto_cnt = -1;
		PWR = `FALSE;
		if(pwrup_check)
		fork
			@(posedge clk or negedge clk) PWR <= #0 `TRUE;
			begin
				#pwrup_time auto_cnt = 0;
				Mode = `INITIAL;
			end
		join
		else
		begin
			@(posedge clk or negedge clk);
			auto_cnt = 2;
			pwrup_done = `TRUE;
			PWR <= #0 `TRUE;
`ifdef v		$display("-- power up check routine skipped");
`endif
		end
	end

/*
 *-----------------------------------------------------
 *	 MRS(mode register set) 
 *-----------------------------------------------------
 */

always	@(modeset)
	begin : mrs_op
		if( &precharge_flag == 1'b1)
		begin
			$display("Warning: Illegal command in precharge operation at %t", CUR_TIME);
			disable mrs_op;
		end

		if (|Mode == 1'b0)
			$display(">> MODE register set at	%t", CUR_TIME);
		else
			begin 
				$display("Warning: Illegal MRS command at %t",CUR_TIME);
				disable mrs_op;
			end

// mode initialization
		MRS_SET    = `FALSE;

		begin
			if(~|RFU)
			begin  // {
				case(addr[2:0]) // burst length programming
					3'b000:begin		
						BL = 1;	
						WBL = 1;	
						$display("-- burst length = 1");
					end
					3'b001:begin
						BL = 2;
						WBL = 2;
						$display("-- burst length = 2");
					end
					3'b010:begin
						BL = 4;
						WBL = 4;
						$display("-- burst length = 4");
					end
					3'b011:begin
						BL = 8;
						WBL = 8;
						$display("-- burst length = 8");
					end
					3'b111:begin // added
						BL = `PAGEDEPTH;
                        WBL = `PAGEDEPTH;
                        $display("-- burst length = %d", `PAGEDEPTH);
                    end
					default	$display("Warning: Invalid Burst length!");
				endcase
`endprotect
	
				if(addr[3] && BL != `PAGEDEPTH)		// burst type  programming
				begin
					burst_type = 1'b1;   
					$display("-- burst type   = interleave.");
				end
				else
				begin
					`ifdef M16G4_M64G3		// jhkim(8.14)
						if (addr[3] && BL == `PAGEDEPTH) begin
						    $display("Warning: interleave mode does not support Full page Mode");
							$display("         interleave mode will be changed to sequential mode");
						end
					`endif
					burst_type = 1'b0;
					$display("-- burst type   = sequential.");
				end
	
				case(addr[6:4])		// CAS latency programming
					3'b001:begin	
					`ifdef NO_CL1
						  $display("Warning: cas latency 1 is not supported in 16M 4Gen. & 64M 3 Gen.");
					`else
						tSAC = `tSAC1; CL=1; tSHZ = `tSHZ1; 
						if( TCC_P < `tCC1-`MARGIN )
							$display("Warning: clock minimun cycle violation at cas latency=1");
						  $display("-- cas latency  = 1");
					`endif
					end 
					3'b010:begin
					`ifdef NO_CL2 //KyW
						  $display("Warning: cas latency 2 is not supported in this device.");
					`else
					   tSAC = `tSAC2; CL=2; tSHZ = `tSHZ2; 
						if( TCC_P < `tCC2-`MARGIN )
							$display("Warning: clock minimun cycle violation at cas latency=2");
					   $display("-- cas latency  = 2");
					`endif
					end
					3'b011:begin
					`ifdef NO_CL3 //KyW
						  $display("Warning: cas latency 3 is not supported in this device.");
					`else
					   tSAC = `tSAC3; CL=3; tSHZ = `tSHZ3; 
						if( TCC_P < `tCC3-`MARGIN )
							$display("Warning: clock minimun cycle violation at cas latency=3");
					   $display("-- cas latency  = 3");
					`endif
					end
					default	$display("Warning: Invalid CAS latency!");
				endcase
	
				if(addr[9])
				begin
					$display("-- Burst read single bit write mode");
					WBL = 1;
				end
`ifdef NOKIA
				REF16M_MODE = `FALSE;
				REF32M_MODE = `FALSE;
`endif
`ifdef MOBILE
				REF4BANK = `TRUE;
				REF2BANK = `FALSE;
				REF1BANK = `FALSE;
`endif
			end 
`ifdef NOKIA
			else if (addr[8:7] === 2'b01) begin
				if (addr[6:4] === 3'b000) begin
					if (addr[3] === 1'b0) begin
						$display("-- Partial Refresh mode (16M)");
						REF16M_MODE = `TRUE;
						REF32M_MODE = `FALSE;
					end
					else if (addr[3] === 1'b1) begin
						$display("-- Partial Refresh mode (32M)");
						REF32M_MODE = `TRUE;
						REF16M_MODE = `FALSE;
					end
				end
				else if (addr[4] === 1'b1) begin
					if (addr[2] === 1'b0) $display("-- VREFi is set to 1.4V");
					if (addr[2] === 1'b1) $display("-- VREFi is set to 0.9V");
				end
			end
`endif
`ifdef MOBILE
		else if (ba[1] === 1'b1 && ba[0] === 1'b0) begin //Extended MRS for Mobile DRAM
			case(addr[6:5]) // Drive Strength
					2'b00:
						$display("-- Full drive strength");
					2'b01:
						$display("-- Half drive strength");
					default	$display("Warning: Invalid drive strength address!");
			endcase
			case(addr[4:3]) // TCSR
					2'b00:
						$display("-- TCSR (46'C~70'C) mode");
					2'b01:
						$display("-- TCSR (16'C~45'C) mode");
					2'b10:
						$display("-- TCSR (-25'C~15'C) mode");
					2'b11:
						$display("-- TCSR (71'C~85'C) mode");
					default	$display("Warning: Invalid TCSR address!");
			endcase
			case(addr[2:0]) // PASR
					3'b000: begin
						$display("-- PASR (4 banks) mode");
						REF4BANK = `TRUE;
						REF2BANK = `FALSE;
						REF1BANK = `FALSE;
					end
					3'b001: begin
						$display("-- PASR (2 banks) mode");
						REF4BANK = `FALSE;
						REF2BANK = `TRUE;
						REF1BANK = `FALSE;
					end
					3'b010: begin
						$display("-- PASR (1 banks) mode");
						REF4BANK = `FALSE;
						REF2BANK = `FALSE;
						REF1BANK = `TRUE;
					end
					default	$display("Warning: Invalid PASR address!");
			endcase
		end
`endif
			else
			begin
				$display("-- reserved for future use !!");
				$display("-- check address: [11,10,8,7] = %b",RFU);
			end

			`ifdef M16G4_M64G3
			  MRS_SET <= repeat (2) @(negedge pclk)  `TRUE; // From 3 -> 2
			`else
			  MRS_SET <= repeat (2) @(negedge pclk)  `TRUE; // From 3 -> 2
			`endif
		end 
	end

`protect
/*
 *-----------------------------------------------------
 *	 ACTIVE command									
 *-----------------------------------------------------
 */

// In active command, bank is selected in accordance with A11 address.

always	@(active)
	begin : active_op
		integer bank_id;
		reg		[8*8:1] str;

		if(CUR_TIME - TRAS_P < `tRRD-`MARGIN)
		begin
			$display("Warning: tRRD violation at %t", CUR_TIME);
			disable active_op;
		end

		r_bank_addr = ba;

		bank_id = BankSelect(ba);
		str = PrintBank(ba);

		//if(CUR_TIME - $bitstoreal(TPRE_P[bank_id]) < `tRP-`MARGIN)
		if ((bank_id == 0) && (CUR_TIME - TPRE_P0 < `tRP-`MARGIN) ||
			(bank_id == 1) && (CUR_TIME - TPRE_P1 < `tRP-`MARGIN) ||
			(bank_id == 2) && (CUR_TIME - TPRE_P2 < `tRP-`MARGIN) ||
			(bank_id == 3) && (CUR_TIME - TPRE_P3 < `tRP-`MARGIN))
		begin
			$display("Warning: tRP violation at %t", CUR_TIME);
			disable active_op;
		end
		//if(CUR_TIME - $bitstoreal(TRAS_PP[bank_id]) < `tRC-`MARGIN)	// 2.27
		if ((bank_id == 0) && (CUR_TIME - TRAS_PP0 < `tRC-`MARGIN) ||
			(bank_id == 1) && (CUR_TIME - TRAS_PP1 < `tRC-`MARGIN) ||
			(bank_id == 2) && (CUR_TIME - TRAS_PP2 < `tRC-`MARGIN) ||
			(bank_id == 3) && (CUR_TIME - TRAS_PP3 < `tRC-`MARGIN))
		begin
			$display("Warning: tRC violation at %t", CUR_TIME);
			disable active_op;
		end

		if(Mode[bank_id] == `TRUE)
		begin
			$display("Warning: Illegal active (%0s is not precharged) at %t",str,CUR_TIME);
		end
		else
		begin
/*
			md = Mode;
			md[bank_id] = 1;
			Mode = md;
*/
			Mode[bank_id] = 1'b1;
`ifdef v		$display(">> active (%0s) at %t", str, CUR_TIME);
`endif
			//TRAS_PP[bank_id] = $realtobits(CUR_TIME);			// save current time for tRCD,tRC check.
			case(bank_id)//TRAS_PP
				'd0:	TRAS_PP0 = CUR_TIME;
				'd1:	TRAS_PP1 = CUR_TIME;
				'd2:	TRAS_PP2 = CUR_TIME;
				'd3:	TRAS_PP3 = CUR_TIME;
			endcase//TRAS_PP

			r_addr_[bank_id] = addr[`ADDRTOP:0];  // check in 64M 2bank
        end
		//TRAS_PP[bank_id] = $realtobits(CUR_TIME);			// save current time for tRCD,tRC check.
			case(bank_id)//TRAS_PP
				'd0:	TRAS_PP0 = CUR_TIME;
				'd1:	TRAS_PP1 = CUR_TIME;
				'd2:	TRAS_PP2 = CUR_TIME;
				'd3:	TRAS_PP3 = CUR_TIME;
			endcase//TRAS_PP
		TRAS_P = CUR_TIME;

	end  // active operation

/*
 *-----------------------------------------------------
 *	 READ command									  
 *-----------------------------------------------------
 */

always	@(read)
	begin :read_block
		integer bank_id;
		reg		[8*8:1] str;

		if(CUR_TIME - TCAS_P < `tCCD-`MARGIN)
		begin
			$display("Warning: tCCD violation at %t", CUR_TIME);
			disable read_block;
		end

	`ifdef M512_M1024_X4
		c_addr = {addr[12:11],addr[`nCOL-3:0]};
	`else
	`ifdef M512_M1024_X8
		c_addr = {addr[11],addr[`nCOL-2:0]};
	`else
	`ifdef M128_M256_X4 // 98.6.30 BYC
		c_addr = {addr[11],addr[`nCOL-2:0]};
	`else
		c_addr = addr[`BIT_C];
	`endif
	`endif
	`endif
		c_bank_addr = ba;

		bank_id = BankSelect(ba);
		str = PrintBank(ba);
		//if(CUR_TIME - $bitstoreal(TRAS_PP[bank_id]) < `tRCD-`MARGIN)
		if ((bank_id == 0) && (CUR_TIME - TRAS_PP0 < `tRCD-`MARGIN) ||
			(bank_id == 1) && (CUR_TIME - TRAS_PP1 < `tRCD-`MARGIN) ||
			(bank_id == 2) && (CUR_TIME - TRAS_PP2 < `tRCD-`MARGIN) ||
			(bank_id == 3) && (CUR_TIME - TRAS_PP3 < `tRCD-`MARGIN))
		begin
			$display("Warning: tRCD violation at %t", CUR_TIME);
			disable read_block;
		end
		r_addr = r_addr_[bank_id];
`endprotect

		if(Mode[bank_id] == `TRUE)
		begin
		    if( addr[`ADDR_AP] == `TRUE)
		    begin
`ifdef v			$display(">> read with auto precharge(%0s) at 	%t",str,CUR_TIME);
`endif
				auto_flag[bank_id] <= @(negedge pclk) `TRUE;
				rd_autoprech_reg <= @(negedge pclk) ba;
			end
		    else begin
`ifdef v		    $display(">> read (%0s) at %t ",str,CUR_TIME);
`endif
				auto_flag[bank_id] <= @(negedge pclk) `FALSE;
			end
		end 

		else begin
`ifdef v		$display("Illegal Read %0s is not activated",str);
`endif
			disable read_block;
		end

		READ_MODE = `TRUE;	// read operation start
		TCAS_P = CUR_TIME;

		m_addr = {r_addr, c_addr};

		if(~burst_type)
			increment_read;
		else	
			interleave_read;

		`ifdef M16G4_M64G3
		`else
		  if( auto_flag[bank_id] )
		  begin
			if( BL != 1)	
 				auto_flagx <= @(negedge pclk) `TRUE;
			->autostart;
		  end
		`endif

		read_task;			// task call

		READ_MODE = `FALSE;   // read operation end.
//        last_rw = CUR_TIME;
		last_rw = TCLK_H;

		if( auto_flag[bank_id] )
		begin
//			rd_autoprech_reg = c_bank_addr;
			if ({CSB_FLAG,RASB_FLAG,CASB_FLAG} == 3'b010 && bank_id === ba)
				$display($time, " Warning: For actual device, this gapless command would be illegal");
			else
				->rd_autoprecharge;
		end
	end

/*
 *-----------------------------------------------------
 *	 WRITE command									 
 *-----------------------------------------------------
 */

//always  @(write)
always  @(posedge write_event)//KyW ... 0408 for VCS
	begin:write_block 
		integer bank_id;
		reg		[8*8:1] str;

		if(CUR_TIME - TCAS_P < `tCCD-`MARGIN)
		begin
			$display("Warning: tCCD violation at %t", CUR_TIME);
			disable write_block;
		end

	`ifdef M512_X4
		c_addr = {addr[12:11],addr[`nCOL-3:0]};
	`else
	`ifdef M512_X8
		c_addr = {addr[11],addr[`nCOL-2:0]};
	`else
	`ifdef M128_M256_X4 // 98.6.30 BYC
		c_addr = {addr[11],addr[`nCOL-2:0]};
	`else
		c_addr = addr[`BIT_C];
	`endif
	`endif
	`endif
		c_bank_addr = ba;

		bank_id = BankSelect(ba);
		str = PrintBank(ba);
		r_addr = r_addr_[bank_id];

		//if(CUR_TIME - $bitstoreal(TRAS_PP[bank_id]) < `tRCD-`MARGIN)
		if ((bank_id == 0) && (CUR_TIME - TRAS_PP0 < `tRCD-`MARGIN) ||
			(bank_id == 1) && (CUR_TIME - TRAS_PP1 < `tRCD-`MARGIN) ||
			(bank_id == 2) && (CUR_TIME - TRAS_PP2 < `tRCD-`MARGIN) ||
			(bank_id == 3) && (CUR_TIME - TRAS_PP3 < `tRCD-`MARGIN))
		begin
			$display("Warning: tRCD violation at %t", CUR_TIME);
			disable write_block;
		end

		if(Mode[bank_id] == `TRUE)
		begin
			if(addr[`ADDR_AP]) 
			begin
`ifdef v			$display(">> write with auto precharge( %0s ) at 	%t",str,CUR_TIME);
`endif
				auto_flag[bank_id]  <= @(negedge pclk) `TRUE;
//`ifdef RDL2
				if (`tRDL == 2)
					wr_autoprech_reg2 <= @(negedge pclk) ba;
//`else
				else
					wr_autoprech_reg <= @(negedge pclk) ba;
//`endif
			end
			else begin
`ifdef v			$display(">> write ( %0s ) at 	%t",str,CUR_TIME);
`endif
				auto_flag[bank_id]  <= @(negedge pclk) `FALSE;
/*
				if (auto_flag == `TRUE && autoprech_reg == ba)
				  auto_flag = `FALSE;
*/
			end
		end
		else 
		begin
			$display("Warning: Illegal write command at %t",CUR_TIME);
			disable write_block;
		end

		WRITE_MODE = `TRUE;
		TCAS_P = CUR_TIME;

		m_addr = {r_addr, c_addr};

		`ifdef M16G4_M64G3
		`else
		  if( auto_flag[bank_id] )
		  begin
			if( WBL != 1)	
 				auto_flagx <= @(negedge pclk) `TRUE;
			->autostart;
		  end
		`endif

		write_task;

		WRITE_MODE <= #(`tSH) `FALSE;
        	last_rw = CUR_TIME;

		@(posedge pclk);
		if(auto_flag[bank_id]) begin
			if ({CSB_FLAG,RASB_FLAG,CASB_FLAG} == 3'b010 && bank_id === ba)
				$display($time, " Warning: For actual device, this gapless command would be illegal");
			else begin
//`ifdef RDL2
				if (`tRDL == 2)
					#0 ->wr_autoprecharge2;
//`else
				else
					->wr_autoprecharge;
//`endif
			end
		end
	end

`protect
// In bank interleave write mode, din data should be stored 
// in din register as the other bank selection occurred.

always	@(flush_write)
	begin
		if(~burst_type)
			increment_write;
		else
			interleave_write;
	end

/*
 *-----------------------------------------------------
 *	 REFRESH command								   
 *-----------------------------------------------------
 */

always	@(autorefresh)
	begin : auto_op
		if (INIT == `FALSE && MRS_SET == `FALSE) // for refersh protection during MRS_ING
		begin
			$display("Warning: Illegal refresh command at %t",CUR_TIME);
		    disable auto_op;
		end

		if (|Mode !== 1'b0) begin
			$display("Warning: Illegal refresh command at %t",CUR_TIME);
			disable auto_op;
		end

/* -- jhkim-TEST
		`ifdef M16G4_M64G3
		`else
		    for(i=0; i < `nBank; i=i+1)
		    begin
			//if( CUR_TIME - $bitstoreal(TRAS_PP[i]) < `tRFC-`MARGIN )
		if ((i == 0) && (CUR_TIME - TRAS_PP0 < `tRFC-`MARGIN) ||
			(i == 1) && (CUR_TIME - TRAS_PP1 < `tRFC-`MARGIN) ||
			(i == 2) && (CUR_TIME - TRAS_PP2 < `tRFC-`MARGIN) ||
			(i == 3) && (CUR_TIME - TRAS_PP3 < `tRFC-`MARGIN))
				$display("Warning: tRFC violation at %t",CUR_TIME);
		    end
		`endif
*/

		AUTOREF_MODE = `TRUE;
`ifdef v
//`ifdef NOKIA
//		if (REF16M_MODE == `TRUE) 
//			$display(">> partial auto refresh (16M) at %t",CUR_TIME);
//		else if (REF32M_MODE == `TRUE)
//			$display(">> partial auto refresh (32M) at %t",CUR_TIME);
//		else
//`endif
			$display(">> auto refresh at       %t",CUR_TIME);
`endif
/* -- jhkim-TEST
		`ifdef M16G4_M64G3
		    AUTOREF_MODE = #(`tRC) `FALSE;
		`else
		    AUTOREF_MODE = #(`tRFC) `FALSE;
		`endif
*/
`ifdef tARFC
		    AUTOREF_MODE = #(`tARFC) `FALSE;
`else
		    AUTOREF_MODE = #(`tRC) `FALSE;
`endif

	end	

/*
 *-----------------------------------------------------
 *	 SELF REFRESH command								   
 *-----------------------------------------------------
 */

always	@(selfrefresh)
	begin : self_op
		if (|Mode == 1'b0) begin
			SELFREF_MODE = `TRUE;
`ifdef v
`ifdef NOKIA
		if (REF16M_MODE == `TRUE)
			$display(">> partial self refresh (16M) enter at %t",CUR_TIME);
		else if (REF32M_MODE == `TRUE)
			$display(">> partial self refresh (32M) enter at %t",CUR_TIME);
		else
`endif
`ifdef MOBILE
		if (REF4BANK == `TRUE)
			$display(">> Partial Array Self Refresh (4banks) enter at %t",CUR_TIME);
		else if (REF2BANK == `TRUE)
			$display(">> Partial Array Self Refresh (2banks) enter at %t",CUR_TIME);
		else if (REF1BANK == `TRUE)
			$display(">> Partial Array Self Refresh (1banks) enter at %t",CUR_TIME);
		else
`endif
			$display(">> self refresh enter at         %t",CUR_TIME);
`endif
			TSELF = CUR_TIME;
		end
		else begin
			$display("Warning: Illegal self refresh command at %t",CUR_TIME);
			disable self_op;
		end

		for(i =0; i < `nBank; i = i+1)
		begin
/* -- jhkim-TEST
			`ifdef M16G4_M64G3
			  //if( CUR_TIME - $bitstoreal(TRAS_PP[i]) < `tRC-`MARGIN )
		if ((i == 0) && (CUR_TIME - TRAS_PP0 < `tRC-`MARGIN) ||
			(i == 1) && (CUR_TIME - TRAS_PP1 < `tRC-`MARGIN) ||
			(i == 2) && (CUR_TIME - TRAS_PP2 < `tRC-`MARGIN) ||
			(i == 3) && (CUR_TIME - TRAS_PP3 < `tRC-`MARGIN))
				$display("Warning: tRC violation at %t",CUR_TIME);
			`else
			  //if( CUR_TIME - $bitstoreal(TRAS_PP[i]) < `tRFC-`MARGIN )
		if ((i == 0) && (CUR_TIME - TRAS_PP0 < `tRFC-`MARGIN) ||
			(i == 1) && (CUR_TIME - TRAS_PP1 < `tRFC-`MARGIN) ||
			(i == 2) && (CUR_TIME - TRAS_PP2 < `tRFC-`MARGIN) ||
			(i == 3) && (CUR_TIME - TRAS_PP3 < `tRFC-`MARGIN))
				$display("Warning: tRFC violation at %t",CUR_TIME);
			`endif
*/
			  //if( CUR_TIME - $bitstoreal(TRAS_PP[i]) < `tRC-`MARGIN )
		if ((i == 0) && (CUR_TIME - TRAS_PP0 < `tRC-`MARGIN) ||
			(i == 1) && (CUR_TIME - TRAS_PP1 < `tRC-`MARGIN) ||
			(i == 2) && (CUR_TIME - TRAS_PP2 < `tRC-`MARGIN) ||
			(i == 3) && (CUR_TIME - TRAS_PP3 < `tRC-`MARGIN))
				$display("Warning: tRC violation at %t",CUR_TIME);
		end
	end	

always @(selfexit) begin
	TSEXIT = CUR_TIME;
/* -- jhkim-TEST
	`ifdef M16G4_M64G3
	  SELFREF_MODE = #(`tRC) `FALSE;
	`else
	  SELFREF_MODE = #(`tRFC) `FALSE;
	`endif
*/
`ifdef tSRFX
		    SELFREF_MODE = #(`tSRFX) `FALSE;
`else
		    SELFREF_MODE = #(`tRC) `FALSE;
`endif
end

/*
always @(negedge cke) begin
	if (SELFREF_MODE == `TRUE && POWERDOWN_MODE == `FALSE) begin
		if (CUR_TIME - TSEXIT < `tSRX-`MARGIN)
			$display("Warning: tSRX violation at %t", CUR_TIME);
	end
end
*/
`endprotect

/*
 *-----------------------------------------------------
 *	 PRECHARGE command								 
 *-----------------------------------------------------
 */

// precharge command performs to disable active operation.

always	@(precharge)
	begin : prech_op
		integer bank_id;
		integer i;
		reg		[8*8:1] str;
		reg		[8*8:1] str1;

		`ifdef NBANK4
		  bank_id = BankSelect(prech_reg[2:1]);
		  str = PrintBank(prech_reg[2:1]);
		`endif
		`ifdef NBANK2
		  bank_id = BankSelect(prech_reg[1]);
		  str = PrintBank(prech_reg[1]);
	    	`endif
`protect

		if(prech_reg[0] == `FALSE)
		begin

		   if(Mode[bank_id] !== 1'b0)
		   begin

                        //if(CUR_TIME - $bitstoreal(TRAS_PP[bank_id]) < `tRASmin-`MARGIN)
		if ((bank_id == 0) && (CUR_TIME - TRAS_PP0 < `tRASmin-`MARGIN) ||
			(bank_id == 1) && (CUR_TIME - TRAS_PP1 < `tRASmin-`MARGIN) ||
			(bank_id == 2) && (CUR_TIME - TRAS_PP2 < `tRASmin-`MARGIN) ||
			(bank_id == 3) && (CUR_TIME - TRAS_PP3 < `tRASmin-`MARGIN))
                        begin
                                $display("Warning: tRASmin violation( %0s ) at %t", str, CUR_TIME);
                                disable prech_op;
                        end

                        //if(first_pre[bank_id]==`FALSE && CUR_TIME - $bitstoreal(TRAS_PP[bank_id]) > `tRASmax+`MARGIN)
		if (first_pre[bank_id]==`FALSE && ((bank_id == 0) && (CUR_TIME - TRAS_PP0 > `tRASmax-`MARGIN) ||
			(bank_id == 1) && (CUR_TIME - TRAS_PP1 > `tRASmax-`MARGIN) ||
			(bank_id == 2) && (CUR_TIME - TRAS_PP2 > `tRASmax-`MARGIN) ||
			(bank_id == 3) && (CUR_TIME - TRAS_PP3 > `tRASmax-`MARGIN)))
                                $display("Warning: tRASmax violation( %0s ) at %t", str, CUR_TIME);

                        first_pre[bank_id] = `FALSE;

				md = Mode;
				md[bank_id] = 0;
				Mode = md;
`ifdef v			$display(">> precharge ( %0s ) at	%t", str, CUR_TIME);
`endif
		    end

		    else
		    begin
`ifdef v			$display("-- current precharge command is NOP at %t",CUR_TIME);
`endif
				disable prech_op;
		    end

			precharge_flag[bank_id] = `TRUE;
			kill_bank = bank_id;
			->precharge_flag_kill;
			//TPRE_P[bank_id] = $realtobits(CUR_TIME);
			case(bank_id)//TPRE_P
				'd0:	TPRE_P0 = CUR_TIME;
				'd1:	TPRE_P1 = CUR_TIME;
				'd2:	TPRE_P2 = CUR_TIME;
				'd3:	TPRE_P3 = CUR_TIME;
			endcase//TPRE_P
		end
	
		else
		begin
			if(|Mode[`nBank-1:0] !== 1'b0)  // BYC
			begin

		        for(i = 0; i < `nBank; i = i+1)
                        begin
                                        case(i)
                                                'd0 :
                                                        str1 = " A Bank";
                                                'd2 :
                                                        str1 = " B Bank";
                                                'd1 :
                                                        str1 = " C Bank";
                                                'd3 :
                                                        str1 = " D Bank";
                                                default :
                                                        str1 = "Bad Bank";
                                        endcase

                                //if(Mode[i] !== 1'b0 && CUR_TIME - $bitstoreal(TRAS_PP[i]) < `tRASmin-`MARGIN)
		if (Mode[i] !== 1'b0 && ((i == 0) && (CUR_TIME - TRAS_PP0 < `tRASmin-`MARGIN) ||
			(i == 1) && (CUR_TIME - TRAS_PP1 < `tRASmin-`MARGIN) ||
			(i == 2) && (CUR_TIME - TRAS_PP2 < `tRASmin-`MARGIN) ||
			(i == 3) && (CUR_TIME - TRAS_PP3 < `tRASmin-`MARGIN)))
                                begin
                                        $display("Warning: tRASmin violation ( %0s ) at %t", str1, CUR_TIME);
                                        disable prech_op;
                                end

                                //if(Mode[i] !== 1'b0 && first_pre[i]==`FALSE && CUR_TIME - $bitstoreal(TRAS_PP[i]) > `tRASmax+`MARGIN)
		if (Mode[i] !== 1'b0 && first_pre[i]==`FALSE && ((i == 0) && (CUR_TIME - TRAS_PP0 > `tRASmax-`MARGIN) ||
			(i == 1) && (CUR_TIME - TRAS_PP1 > `tRASmax-`MARGIN) ||
			(i == 2) && (CUR_TIME - TRAS_PP2 > `tRASmax-`MARGIN) ||
			(i == 3) && (CUR_TIME - TRAS_PP3 > `tRASmax-`MARGIN)))
                                        $display("Warning: tRASmax violation ( %0s ) at %t", str1, CUR_TIME);

                                first_pre[i] = `FALSE;

                                Mode[i] = 1'b0;
                                precharge_flag[i] = `TRUE;
                                //TPRE_P[i] = $realtobits(CUR_TIME);
			case(i)//TPRE_P
				'd0:	TPRE_P0 = CUR_TIME;
				'd1:	TPRE_P1 = CUR_TIME;
				'd2:	TPRE_P2 = CUR_TIME;
				'd3:	TPRE_P3 = CUR_TIME;
			endcase//TPRE_P
                                first_pre[i] = `FALSE;
                        end
			
			`ifdef NBANK2
`ifdef v			$display(">> precharge ( A and B bank ) at 	%t",CUR_TIME);
`endif
			`endif
			`ifdef NBANK4
`ifdef v			$display(">> precharge ( A,B,C, and D bank ) at 	%t",CUR_TIME);
`endif
			`endif
			end

			else
			begin
`ifdef v			$display("-- current precharge command is NOP at %t",CUR_TIME);
`endif
				disable prech_op;
			end

			kill_bank = bank_id;
			->precharge_flag_kill;

		end
		->precharge_start;
	end
`endprotect

/*
 *-----------------------------------------------------
 *	 tRDL=2 AUTO PRECHARGE command	 
 *-----------------------------------------------------
 */

always @(wr_autoprecharge2) begin
	wr_autoprech_reg = wr_autoprech_reg2;
	@(posedge pclk);
	if ({CSB_FLAG,RASB_FLAG,CASB_FLAG} == 3'b010 && wr_autoprech_reg === ba)
		$display($time, " Warning: For actual device, this 1-clk-gap command would be illegal");
	else
		->wr_autoprecharge;
end

/*
 *-----------------------------------------------------
 *	 READ AUTO PRECHARGE command								 
 *-----------------------------------------------------
 */

always	@(rd_autoprecharge or posedge rd_reautoprecharge)
begin : rd_autoprech_op
real difftime;
integer bank_id;
reg		[8*8:1] str;
reg		tmp_reauto;
integer	prev_bank;
integer tmp_bank;

	tmp_reauto = `FALSE;
	bank_id = BankSelect(rd_autoprech_reg);
	if (rd_reautoprecharge == `TRUE) begin
		rd_reautoprecharge = `FALSE;
		tmp_bank = prev_bank;
	end
	else
		tmp_bank = bank_id;
	str = PrintBank(tmp_bank);
	//difftime = $realtime - $bitstoreal(TRAS_PP[tmp_bank]);
			case(tmp_bank)//TRAS_PP
				'd0:	difftime = $realtime-TRAS_PP0;
				'd1:	difftime = $realtime-TRAS_PP1;
				'd2:	difftime = $realtime-TRAS_PP2;
				'd3:	difftime = $realtime-TRAS_PP3;
			endcase//TRAS_PP
	if(difftime < `tRASmin-`MARGIN)
	begin
	  `ifdef M64G3_M128_M256
		//auto_flagx <= `TRUE;				// KyW ... 0624 : rev 3.3
		//auto_flagx <= #(`tRASmin-difftime) `FALSE;	// KyW ... 0624 : rev 3.3
		  prev_bank <= #(`tRASmin-difftime) tmp_bank;
		  rd_reautoprecharge <= #(`tRASmin-difftime) `TRUE;
		  tmp_reauto = `TRUE;
`ifdef v	  $display(" Info: Staring Auto precharge (%s) delayed by tRASmin violation at %t", str, $time);
`endif
//		  #0 disable rd_autoprech_op;
	  `else
		rd_reautoprecharge = `FALSE;
		$display("Warning: tRASmin violation at %t", $realtime);
		disable rd_autoprech_op;
	  `endif
	end
	if(difftime > `tRASmax+`MARGIN)
	begin
		$display("Warning: tRASmax violation at %t", $realtime);
	end

	`ifdef M16G4_M64G3
		if (tmp_reauto == `FALSE) begin
	`endif
`ifdef v	$display(">> auto precharge ( %0s) at	%t", str, $realtime);
`endif
	Mode[tmp_bank] = 0;

	precharge_flag[tmp_bank] = `TRUE;
	rd_kill_bank = tmp_bank;
		->rd_precharge_flag_kill;
	//TPRE_P[tmp_bank] = $realtobits($realtime);
			case(tmp_bank)//TPRE_P
				'd0:	TPRE_P0 = $realtime;
				'd1:	TPRE_P1 = $realtime;
				'd2:	TPRE_P2 = $realtime;
				'd3:	TPRE_P3 = $realtime;
			endcase//TPRE_P
	`ifdef M16G4_M64G3
		end
	`endif
end

/*
 *-----------------------------------------------------
 *	 WRITE AUTO PRECHARGE command								 
 *-----------------------------------------------------
 */

always	@(wr_autoprecharge or posedge wr_reautoprecharge)
begin : wr_autoprech_op
real difftime;
integer bank_id;
reg		[8*8:1] str;
reg		tmp_reauto;
integer	prev_bank;
integer tmp_bank;

	tmp_reauto = `FALSE;
	bank_id = BankSelect(wr_autoprech_reg);
	if (wr_reautoprecharge == `TRUE) begin
		wr_reautoprecharge = `FALSE;
		tmp_bank = prev_bank;
	end
	else
		tmp_bank = bank_id;
	str = PrintBank(tmp_bank);
	//difftime = $realtime - $bitstoreal(TRAS_PP[tmp_bank]);
			case(tmp_bank)//TRAS_PP
				'd0:	difftime = $realtime-TRAS_PP0;
				'd1:	difftime = $realtime-TRAS_PP1;
				'd2:	difftime = $realtime-TRAS_PP2;
				'd3:	difftime = $realtime-TRAS_PP3;
			endcase//TRAS_PP
	if(difftime < `tRASmin-`MARGIN)
	begin
	  `ifdef M64G3_M128_M256
		//auto_flagx <= `TRUE;				// KyW ... 0624 : rev 3.3
		//auto_flagx <= #(`tRASmin-difftime) `FALSE;	// KyW ... 0624 : rev 3.3
		  prev_bank <= #(`tRASmin-difftime) tmp_bank;
		  wr_reautoprecharge <= #(`tRASmin-difftime) `TRUE;
		  tmp_reauto = `TRUE;
`ifdef v	  $display(" Info: Staring Auto precharge (%s) delayed by tRASmin violation at %t", str, $time);
`endif
//		  #0 disable wr_autoprech_op;
	  `else
		wr_reautoprecharge = `FALSE;
		$display("Warning: tRASmin violation at %t", $realtime);
		disable wr_autoprech_op;
	  `endif
	end
	if(difftime > `tRASmax+`MARGIN)
	begin
		$display("Warning: tRASmax violation at %t", $realtime);
	end

	`ifdef M16G4_M64G3
		if (tmp_reauto == `FALSE) begin
	`endif
`ifdef v	$display(">> auto precharge ( %0s) at	%t", str, $realtime);
`endif
	Mode[tmp_bank] = 0;

	precharge_flag[tmp_bank] = `TRUE;
	wr_kill_bank = tmp_bank;
		->wr_precharge_flag_kill;
	//TPRE_P[tmp_bank] = $realtobits($realtime);
			case(tmp_bank)//TPRE_P
				'd0:	TPRE_P0 = $realtime;
				'd1:	TPRE_P1 = $realtime;
				'd2:	TPRE_P2 = $realtime;
				'd3:	TPRE_P3 = $realtime;
			endcase//TPRE_P
	`ifdef M16G4_M64G3
		end
	`endif
end

/*
 *-----------------------------------------------------
 */

always @(autostart)
	begin
		if( READ_MODE )
		begin
			auto_flagx = repeat (BL) @(negedge pclk) `FALSE;
		end
		else if( WRITE_MODE )
		begin
			auto_flagx = repeat (WBL) @(negedge pclk) `FALSE;
		end
	end
/*
 *-----------------------------------------------------
 *	 DEEP POWER DOWN
 *-----------------------------------------------------
 */
`ifdef DPD
always	@(deeppowerdown)
	begin : d_powerdown
		if (|Mode == 1'b0) begin
			D_POWERDOWN = `TRUE;
`ifdef v
			$display(">> Deep power down enter at         %t",CUR_TIME);
`endif
		end
		else begin
			$display("Warning: Illegal deep power down command at %t",CUR_TIME);
			disable d_powerdown;
		end
	end	

always @(d_pdown_exit) begin
	D_POWERDOWN <= #(`tDPDEXIT-`MARGIN) `FALSE;
	D_PDOWN_EXIT <= #(`tDPDEXIT-`MARGIN) `TRUE;
end
`endif

/*
 *-----------------------------------------------------
 *	 move memory data to dout register				  
 *	   by sequential counter
 *-----------------------------------------------------
 */

// This task models behavior of increment counter
// Simply, address is increased by one and one.

task	increment_read;
	begin:ir
		integer j,s,t;
		integer	bank;
        reg [`BIT_T]  maddr;
		reg [`BIT_C] check_111;

		bank = BankSelect(c_bank_addr);

        maddr = m_addr;
		for(j=0; j<= BL-1; j=j+1) begin

			case(bank)
				'd0: begin
				`ifdef DYMEM
					$damem_read("mem_a", maddr, dout_reg[j]);
					$damem_read("mem_a", maddr, din_rega[j]);
				`else
					dout_reg[j] = mem_a[maddr];
					din_rega[j] = mem_a[maddr];
				`endif
				end
				'd1: begin
				`ifdef DYMEM
					`ifdef MOBILE
						if (REF1BANK == `TRUE) dout_reg[j] = `B'bx;
						else
						$damem_read("mem_b", maddr, dout_reg[j]);
					`else
						$damem_read("mem_b", maddr, dout_reg[j]);
					`endif
					$damem_read("mem_b", maddr, din_regb[j]);
				`else
					`ifdef MOBILE
						if (REF1BANK == `TRUE) dout_reg[j] = `B'bx;
						else
						dout_reg[j] = mem_b[maddr];
					`else
						dout_reg[j] = mem_b[maddr];
					`endif
					din_regb[j] = mem_b[maddr];
				`endif
				end
				`ifdef NBANK4
				  'd2: begin
				`ifdef DYMEM
					`ifdef MOBILE
						if (REF4BANK == `FALSE) dout_reg[j] = `B'bx;
						else
						$damem_read("mem_c", maddr, dout_reg[j]);
					`else
						$damem_read("mem_c", maddr, dout_reg[j]);
					`endif
					$damem_read("mem_c", maddr, din_regc[j]);
				`else
					`ifdef MOBILE
						if (REF4BANK == `FALSE) dout_reg[j] = `B'bx;
						else
						dout_reg[j] = mem_c[maddr];
					`else
						dout_reg[j] = mem_c[maddr];
					`endif
					din_regc[j] = mem_c[maddr];
				`endif
				  end
				  'd3: begin
				`ifdef DYMEM
					`ifdef MOBILE
						if (REF4BANK == `FALSE) dout_reg[j] = `B'bx;
						else
						$damem_read("mem_d", maddr, dout_reg[j]);
					`else
						$damem_read("mem_d", maddr, dout_reg[j]);
					`endif
					$damem_read("mem_d", maddr, din_regd[j]);
				`else
					`ifdef MOBILE
						if (REF4BANK == `FALSE) dout_reg[j] = `B'bx;
						else
						dout_reg[j] = mem_d[maddr];
					`else
						dout_reg[j] = mem_d[maddr];
					`endif
					din_regd[j] = mem_d[maddr];
				`endif
				  end
				`endif
			endcase

			case(BL)
				'd1: begin
				end
				'd2: maddr[0] = ~maddr[0];
				'd4: begin
					check_111 = m_addr + j+1;
					maddr[1:0] = check_111[1:0];
				end
				'd8: begin 
					check_111 = m_addr + j+1;
					maddr[2:0] = check_111[2:0];
				end
				`PAGEDEPTH: begin    // case 256
					check_111 = m_addr + j+1;
					maddr[`BIT_C] = check_111[`BIT_C];
            	end
				default: begin
					$display("Warning: burst length is out of spec");
					disable increment_read;
				end
			endcase
		end // end of for loop

	end	
endtask

/*
 *-----------------------------------------------------
 *	 move memory data to dout register				  
 *	   by interleave counter
 *-----------------------------------------------------
 */

// Interleave counting mechanism is different from 
// sequential method. Counting step could be varied with
// initial address.(refer to data sheet)

task	interleave_read;
	begin:ir1
	integer j;
	integer bank;
    reg [`BIT_T] maddr;

		bank = BankSelect(c_bank_addr);
        maddr = m_addr;
		for(j=0; j<=BL-1; j=j+1) begin
			case(bank)
				'd0: begin
				`ifdef DYMEM
					$damem_read("mem_a", maddr, dout_reg[j]);
					$damem_read("mem_a", maddr, din_rega[j]);
				`else
					dout_reg[j] = mem_a[maddr];
					din_rega[j] = mem_a[maddr];
				`endif
				end
				'd1: begin
				`ifdef DYMEM
					`ifdef MOBILE
						if (REF1BANK == `TRUE) dout_reg[j] = `B'bx;
						else
						$damem_read("mem_b", maddr, dout_reg[j]);
					`else
						$damem_read("mem_b", maddr, dout_reg[j]);
					`endif
					$damem_read("mem_b", maddr, din_regb[j]);
				`else
					`ifdef MOBILE
						if (REF1BANK == `TRUE) dout_reg[j] = `B'bx;
						else
						dout_reg[j] = mem_b[maddr];
					`else
						dout_reg[j] = mem_b[maddr];
					`endif
					din_regb[j] = mem_b[maddr];
				`endif
				end
				`ifdef NBANK4
				  'd2: begin
				`ifdef DYMEM
					`ifdef MOBILE
						if (REF4BANK == `FALSE) dout_reg[j] = `B'bx;
						else
						$damem_read("mem_c", maddr, dout_reg[j]);
					`else
						$damem_read("mem_c", maddr, dout_reg[j]);
					`endif
					$damem_read("mem_c", maddr, din_regc[j]);
				`else
					`ifdef MOBILE
						if (REF4BANK == `FALSE) dout_reg[j] = `B'bx;
						else
						dout_reg[j] = mem_c[maddr];
					`else
						dout_reg[j] = mem_c[maddr];
					`endif
					din_regc[j] = mem_c[maddr];
				`endif
				  end
				  'd3: begin
				`ifdef DYMEM
					`ifdef MOBILE
						if (REF4BANK == `FALSE) dout_reg[j] = `B'bx;
						else
						$damem_read("mem_d", maddr, dout_reg[j]);
					`else
						$damem_read("mem_d", maddr, dout_reg[j]);
					`endif
					$damem_read("mem_d", maddr, din_regd[j]);
				`else
					`ifdef MOBILE
						if (REF4BANK == `FALSE) dout_reg[j] = `B'bx;
						else
						dout_reg[j] = mem_d[maddr];
					`else
						dout_reg[j] = mem_d[maddr];
					`endif
					din_regd[j] = mem_d[maddr];
				`endif
				  end
				`endif
			endcase

			case(BL)
				'd1:begin
				end
				'd2: 
						maddr[0] = ~maddr[0];
				'd4: begin
					if( j == 0 || j == 2)
						maddr[0] = ~maddr[0];
					else 
						maddr[1:0] = ~maddr[1:0];
				end
				'd8: begin
					if(j == 0 || j == 2 || j == 4 || j==6)
						maddr[0] = ~maddr[0];
					else if(j == 1 || j == 5)
						maddr[1:0] = ~maddr[1:0];
					else
						maddr[2:0] = ~maddr[2:0];
				end
				default: $display("Warning: burst length is out of spec.");
			endcase
		end
	end
endtask

/*
 *-----------------------------------------------------
 *	 move memory data to din register array			 
 *	   by sequential counter
 *-----------------------------------------------------
 */

task	increment_write;
	begin:iw
		integer j,s,t;
        reg [`BIT_T] maddr;
		reg [`BIT_C] check_111;
		integer bank;

		bank = BankSelect(c_bank_addr);
        maddr = m_addr;
		for(j=0; j<=WBL-1; j=j+1) begin
			case(bank)
				'd0:
				`ifdef DYMEM
					$damem_write("mem_a", maddr, din_rega[j]);
				`else
					mem_a[maddr] = din_rega[j];
				`endif
				'd1:
				`ifdef DYMEM
					$damem_write("mem_b", maddr, din_regb[j]);
				`else
					mem_b[maddr] = din_regb[j];
				`endif
				`ifdef NBANK4
				  'd2:
				`ifdef DYMEM
					$damem_write("mem_c", maddr, din_regc[j]);
				`else
					mem_c[maddr] = din_regc[j];
				`endif
				  'd3:
				`ifdef DYMEM
					$damem_write("mem_d", maddr, din_regd[j]);
				`else
					mem_d[maddr] = din_regd[j];
				`endif
				`endif
			endcase
			case(WBL)	
				'd1: begin
				end
				'd2: 
			 		maddr[0] = ~maddr[0];
				'd4: begin
					check_111 = m_addr +j+1;
					maddr[1:0] = check_111[1:0];
				end
				'd8: begin 
					check_111 = m_addr +j+1;
					maddr[2:0] = check_111[2:0];
				end
				`PAGEDEPTH: begin 
					check_111 = m_addr +j+1;
					maddr[`BIT_C] = check_111[`BIT_C];
            	end
				default: begin
					$display("Warning: burst length is out of spec");
					disable increment_write;
				end
			endcase
		end
	end	
endtask

/*
 *-----------------------------------------------------
 *	 move memory data to din register array			 
 *	   by interleave counter
 *-----------------------------------------------------
 */

task	interleave_write;
	begin:iw1
	integer j;
	integer bank;
    reg [`BIT_T] maddr;

	bank = BankSelect(c_bank_addr);
    maddr = m_addr;
	for(j=0; j <= WBL-1; j=j+1) begin
			case(bank)
				'd0: 
				`ifdef DYMEM
					$damem_write("mem_a", maddr, din_rega[j]);
				`else
					mem_a[maddr] = din_rega[j];
				`endif
				'd1: 
				`ifdef DYMEM
					$damem_write("mem_b", maddr, din_regb[j]);
				`else
					mem_b[maddr] = din_regb[j];
				`endif
				`ifdef NBANK4
				  'd2: 
				`ifdef DYMEM
					$damem_write("mem_c", maddr, din_regc[j]);
				`else
					mem_c[maddr] = din_regc[j];
				`endif
				  'd3: 
				`ifdef DYMEM
					$damem_write("mem_d", maddr, din_regd[j]);
				`else
					mem_d[maddr] = din_regd[j];
				`endif
				`endif
			endcase
			case(WBL)
				'd1:begin
                end
                'd2:
                        maddr[0] = ~maddr[0];
                'd4: begin
                    if((j % 2) == 0)
                        maddr[0] = ~maddr[0];
                    else
                        maddr[1:0] = ~maddr[1:0];
                end
                'd8: begin
                    if((j % 2) == 0)
                        maddr[0] = ~maddr[0];
                    else if(j == 1 || j == 5)
                        maddr[1:0] = ~maddr[1:0];
                    else
                        maddr[2:0] = ~maddr[2:0];
                end
				default: 
				begin
					$display("Warning: burst length is out of spec.");
				end
			endcase
		end
	end
endtask

`protect

/*
 *-----------------------------------------------------
 *   precharge interrupt
 *-----------------------------------------------------
 */
always @(precharge_start)
	begin: pc_start
	integer bank_id;
	reg		[8*8:1] str;

		if( READ_MODE == `TRUE )
		begin
			bank_id = BankSelect(c_bank_addr);
			str = PrintBank(c_bank_addr);
			if(precharge_flag[bank_id])
			begin
`ifdef v		$display("-- read operation interrupted by precharge");
`endif
				READ_MODE = `FALSE;
				disable read_block;
			end
		end
		if( WRITE_MODE == `TRUE )
		begin
			bank_id = BankSelect(c_bank_addr);
			str = PrintBank(c_bank_addr);
			if(precharge_flag[bank_id])
			begin
`ifdef v			$display("-- write operation interrupted by precharge");
`endif
				for (i = 0; i < `nDQM; i = i + 1) begin : dqm_high_check
				  if(dqm[i] == `FALSE) begin
`ifdef v				$display("   DQM must be high at %t", CUR_TIME);
`endif
					i = `nDQM;
				  end
				end
				WRITE_MODE = `FALSE;
				disable write_block;
			end
		end
	end

always @(precharge_flag_kill) begin
	if (prech_reg[0] == `TRUE) begin
		for (i = 0; i < `nBank; i = i+1)
			precharge_flag[i] <= #(`tRP-1) `FALSE;
	end
	else begin
		if( precharge_flag[kill_bank] )
			precharge_flag[kill_bank] <= #(`tRP-1) `FALSE;
	end
end
always @(rd_precharge_flag_kill) begin
	if (prech_reg[0] == `TRUE) begin
		for (i = 0; i < `nBank; i = i+1)
			precharge_flag[i] <= #(`tRP-1) `FALSE;
	end
	else begin
		if( precharge_flag[rd_kill_bank] )
			precharge_flag[rd_kill_bank] <= #(`tRP-1) `FALSE;
	end
end
always @(wr_precharge_flag_kill) begin
	if (prech_reg[0] == `TRUE) begin
		for (i = 0; i < `nBank; i = i+1)
			precharge_flag[i] <= #(`tRP-1) `FALSE;
	end
	else begin
		if( precharge_flag[wr_kill_bank] )
			precharge_flag[wr_kill_bank] <= #(`tRP-1) `FALSE;
	end
end
`endprotect

/*
 *-----------------------------------------------------
 *	 read task 
 *-----------------------------------------------------
 */

task read_task;

	begin

		begin: read_op
			integer i;
	
			for( i=0; i < BL; i=i+1 )
			begin
				t_dqo = dout_reg[i];
				->dqo_event;
				@(posedge pclk);
				`ifdef M16G4_M64G3	// jhkim(8.14)
					if( i == `PAGEDEPTH - 1 )  
						i = -1; 			// full page wrap around
				`endif
			end
		end

	end
endtask

/*
 *-----------------------------------------------------
 *	 write task
 *-----------------------------------------------------
 */

task write_task;
	begin
		begin: write_op
			integer    i, j, k;
			reg [`BIT] tmp_reg;
			integer bank_id;
			reg		[8*8:1] str;

			if(~burst_type)
				increment_read;
			else
				interleave_read;

			begin: write_seq
				for(i = 0; i < WBL; i = i+1)
				begin  // { for loop
					begin
						bank_id = BankSelect(c_bank_addr);
						str = PrintBank(c_bank_addr);
						if(precharge_flag[bank_id] == `TRUE)
							disable write_seq;
						case (bank_id)
							'd0: tmp_reg = din_rega[i];
							'd1: tmp_reg = din_regb[i];
							`ifdef NBANK4
							  'd2: tmp_reg = din_regc[i];
							  'd3: tmp_reg = din_regd[i];
							`endif
						endcase

						
						for (k = 0; k < `nDQM; k = k + 1) begin
						  if (dqm[k] == 1'b0) begin
							for (j = k*`BYTE; j < (k+1)*`BYTE; j = j + 1) begin
							   tmp_reg[j] = (dqi[j] == 1'b1 || dqi[j] == 1'b0)?
										    dqi[j]:1'bx;
						     end
						   end
						end

						case (bank_id)
							'd0: din_rega[i] = tmp_reg;
							'd1: din_regb[i] = tmp_reg;
							`ifdef NBANK4
							  'd2: din_regc[i] = tmp_reg;
							  'd3: din_regd[i] = tmp_reg;
							`endif
						endcase
					end

					m_addr = {r_addr, c_addr};
					#0 ->flush_write;

					`ifdef M16G4_M64G3    // jhkim(8.14)
						if( i == WBL-1 && WBL !== `PAGEDEPTH )   
                        				disable write_seq;
					`else
						if( i == WBL-1 && i != `PAGEDEPTH )
							disable write_seq;
					`endif
					@(posedge pclk);
					#0.1; //KyW ... 0408 for VCS
					if( |dqm === 1'b0 ) begin // 98.6.26 BYC
					if( CUR_TIME - TDQI < `tSS-`MARGIN )
						$display("Warning: DQi setup violation at %t", CUR_TIME);
					end
					`ifdef M16G4_M64G3    // jhkim(8.14)
						if ( i == `PAGEDEPTH -1) 
							i = -1; // full page wrap around
					`endif

				end  // } for loop end
			end  // write_seq

//			m_addr = {r_addr, c_addr};
//			->flush_write;

		end

	end

endtask 

`ifdef NBANK2
function integer BankSelect;
input	 	c_addr;
integer		bank;
begin
	case(c_addr)
		1'b0 :
			bank = 0;
		1'b1 :
			bank = 1;
		default :
			bank = -1;
	endcase
	BankSelect = bank;
end
endfunction
function [8*8 : 1] PrintBank;
input				bs;
reg		[8*8 : 1]	s_bank;
begin
	case(bs)
		1'b0 :
			s_bank = " A Bank";
		1'b1 :
			s_bank = " B Bank";
		default :
			s_bank = "Bad Bank";
	endcase
	PrintBank = s_bank;
end
endfunction
`endif

`ifdef NBANK4
function integer BankSelect;
input	[1:0] 	c_addr;
integer			bank;
begin
	case(c_addr)
		2'b00 :
			bank = 0;
		2'b01 :
			bank = 1;
		2'b10 :
			bank = 2;
		2'b11 :
			bank = 3;
		default :
			bank = -1;
	endcase
	BankSelect = bank;
end
endfunction
function [8*8 : 1] PrintBank;
input	[1:0]	bs;
reg		[8*8 : 1]	s_bank;
begin
	case(bs)
		2'b00 :
			s_bank = " A Bank";
		2'b01 :
			s_bank = " B Bank";
		2'b10 :
			s_bank = " C Bank";
		2'b11 :
			s_bank = " D Bank";
		default :
			s_bank = "Bad Bank";
	endcase
	PrintBank = s_bank;
end
endfunction
`endif

endmodule

`ifdef CS2
	`ifdef DPD_PIN
		module sdram(clk, cs0b, cs1b, cke, ba, ad, rasb, casb, web, dqm, dqi, dpdb);
	`else
		module sdram(clk, cs0b, cs1b, cke, ba, ad, rasb, casb, web, dqm, dqi);
	`endif

		  inout   [`BIT]  dqi;
		  input   [`nBank/2-1:0] ba;
		  input   [`ADDRTOP:0]  ad;
		  input   rasb,casb,web;
		  input   clk,cke,cs0b,cs1b;
		  input   [`nDQM-1:0] dqm;

		`ifdef DPD_PIN
		  input   dpdb;
		`endif

		`ifdef DPD_PIN
			sdram_cs2 i0(clk, cs0b, cke, ba, ad, rasb, casb, web, dqm, dqi, dpdb);
			sdram_cs2 i1(clk, cs1b, cke, ba, ad, rasb, casb, web, dqm, dqi, dpdb);
		`else
			sdram_cs2 i0(clk, cs0b, cke, ba, ad, rasb, casb, web, dqm, dqi);
			sdram_cs2 i1(clk, cs1b, cke, ba, ad, rasb, casb, web, dqm, dqi);
		`endif
		endmodule
`endif

`ifdef CS2_CKE2
        `ifdef DPD_PIN
                module sdram(clk, cs0b, cs1b, cke0, cke1, ba, ad, rasb, casb, web, dqm, dqi, dpdb);
        `else
                module sdram(clk, cs0b, cs1b, cke0, cke1, ba, ad, rasb, casb, web, dqm, dqi);
        `endif

                  inout   [`BIT]  dqi;
                  input   [`nBank/2-1:0] ba;
                  input   [`ADDRTOP:0]  ad;
                  input   rasb,casb,web;
                  input   clk,cke0,cke1,cs0b,cs1b;
                  input   [`nDQM-1:0] dqm;

                `ifdef DPD_PIN
                  input   dpdb;
                `endif

                `ifdef DPD_PIN
                        sdram_cs2 i0(clk, cs0b, cke0, ba, ad, rasb, casb, web, dqm, dqi, dpdb);
                        sdram_cs2 i1(clk, cs1b, cke1, ba, ad, rasb, casb, web, dqm, dqi, dpdb);
                `else
                        sdram_cs2 i0(clk, cs0b, cke0, ba, ad, rasb, casb, web, dqm, dqi);
                        sdram_cs2 i1(clk, cs1b, cke1, ba, ad, rasb, casb, web, dqm, dqi);
                `endif
                endmodule
`endif

