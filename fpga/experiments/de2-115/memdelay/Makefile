#
# Makefile for testing the memory delay
#

all:		show

show:		memdelay
		./memdelay
		gtkwave dump.vcd memdelay.cfg

memdelay:	memdelay.v ram.v
		iverilog -Wall -o memdelay memdelay.v ram.v

clean:
		rm -f *~ memdelay dump.vcd
