#
# Makefile for pipeline simulation of ECO32
#

BUILD = ../../build

SRCS = eco32test.v \
       src/toplevel/eco32.v \
       src/clk_rst/clk_rst.v \
       src/cpu/cpu.v \
       src/cpu/if/if1a.v src/cpu/if/if1b.v src/cpu/if/if2.v
BIN = eco32test

.PHONY:		all install run show clean

all:		show

install:	$(BIN)

$(BIN):		$(SRCS)
		iverilog -Wall -o $(BIN) $(SRCS)

run:		$(BIN)
		@if [ ! -r duration.dat ] ; then \
		  echo "cp defdata/duration.dat ." ; \
		  cp defdata/duration.dat . ; \
		fi
		./$(BIN)

show:		run
		gtkwave dump.vcd eco32test.cfg

clean:
		rm -f *~ $(BIN) dump.vcd
		rm -f duration.dat
		rm -f defdata/*~
		rm -f src/*~
		rm -f src/toplevel/*~
		rm -f src/clk_rst/*~
		rm -f src/cpu/*~
		rm -f src/cpu/if/*~
		rm -f src/cpu/of/*~
		rm -f src/cpu/ex/*~
		rm -f src/cpu/ma/*~
		rm -f src/cpu/wb/*~
