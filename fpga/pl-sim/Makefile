#
# Makefile for pipeline simulation of ECO32
#

BUILD = ../../build

DEF_DURATION = defdata/duration00.dat
DEF_ROM = defdata/romrand.dat

SRCS = eco32test.v \
       src/toplevel/eco32.v \
       src/clk_rst/clk_rst.v \
       src/cpu/cpu.v \
       src/cpu/if/if1a.v src/cpu/if/if1b.v src/cpu/if/if2.v src/cpu/if/if3.v \
       src/cpu/test/if/test.v \
       src/ramctrl/ramctrl.v src/ramctrl/ram.v \
       src/romctrl/romctrl.v src/romctrl/rom.v
BIN = eco32test

.PHONY:		all install makedef run show clean

all:		show

install:	$(BIN)

makedef:
		$(MAKE) -C defdata all

$(BIN):		$(SRCS)
		iverilog -Wall -o $(BIN) $(SRCS)

run:		makedef $(BIN)
		@if [ ! -r duration.dat ] ; then \
		  echo "cp $(DEF_DURATION) duration.dat" ; \
		  cp $(DEF_DURATION) duration.dat ; \
		fi
		@if [ ! -r rom.dat ] ; then \
		  echo "cp $(DEF_ROM) rom.dat" ; \
		  cp $(DEF_ROM) rom.dat ; \
		fi
		./$(BIN)

show:		run
		gtkwave dump.vcd eco32test.cfg

clean:
		$(MAKE) -C defdata clean
		rm -f *~ $(BIN) dump.vcd
		rm -f duration.dat rom.dat
		rm -f src/*~
		rm -f src/toplevel/*~
		rm -f src/clk_rst/*~
		rm -f src/cpu/*~
		rm -f src/cpu/if/*~
		rm -f src/cpu/of/*~
		rm -f src/cpu/ex/*~
		rm -f src/cpu/ma/*~
		rm -f src/cpu/wb/*~
		rm -f src/cpu/test/*~
		rm -f src/cpu/test/if/*~
		rm -f src/ramctrl/*~
		rm -f src/romctrl/*~
