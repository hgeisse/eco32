#
# Makefile for testing the pipeline
#

SRCS = pipetest.v pipe.v
BIN = pipetest

.PHONY:		all install run show clean

all:		show

install:

$(BIN):		$(SRCS)
		iverilog -Wall -o $(BIN) $(SRCS)

run:		$(BIN)
		./$(BIN)

show:		run
		gtkwave dump.vcd $(BIN).cfg

clean:
		rm -f *~ $(BIN) dump.vcd
