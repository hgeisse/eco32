#
# Makefile for testing floating-point arithmetic
#

SRCS = fptest.v fpcmp/fpcmp.v

all:		fptest fpserver

show:		run
		gtkwave dump.vcd fptest.cfg

run:		fptest
		./fptest <input >output

fptest:		$(SRCS)
		iverilog -Wall -o fptest -DGENDUMP $(SRCS)

fpserver:	$(SRCS)
		iverilog -Wall -o fpserver $(SRCS)

clean:
		rm -f *~
		rm -f dump.vcd fptest fpserver output
