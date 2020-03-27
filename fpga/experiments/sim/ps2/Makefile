#
# Makefile for testing PS/2 communication
#

SRCS = ps2comm.v device.v host.v

all:		show

show:		ps2comm
		./ps2comm
		gtkwave dump.vcd ps2comm.cfg

ps2comm:	$(SRCS)
		iverilog -Wall -o ps2comm $(SRCS)

clean:
		rm -f *~ ps2comm dump.vcd
