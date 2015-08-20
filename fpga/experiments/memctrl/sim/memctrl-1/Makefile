#
# Makefile for testing the memory controller
#

all:		show

install:

memtest:	memtest.v
		iverilog -Wall -o memtest memtest.v

show:		memtest
		./memtest
		gtkwave dump.vcd memtest.cfg

clean:
		rm -f *~ memtest dump.vcd
		rm -f ramtest/*~ ramctrl/*~
