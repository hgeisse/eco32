#
# Makefile for SD card test
#

all:		show

show:		sdcbench
		./sdcbench
		gtkwave dump.vcd sdcbench.cfg

sdcbench:	sdcbench.v
		iverilog -Wall -o sdcbench sdcbench.v

clean:
		rm -f *~ sdcbench dump.vcd
