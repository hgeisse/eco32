#
# Makefile for testing democache
#

SRCS = democache.v cachetest.v cachectrl.v memory.v

all:		show

democache:	$(SRCS)
		iverilog -Wall -o democache $(SRCS)

show:		democache
		./democache
		gtkwave dump.vcd democache.cfg

clean:
		rm -f *~ democache dump.vcd
