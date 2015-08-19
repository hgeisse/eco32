#
# Makefile for multicycle simulation of ECO32, using Verilator
#

BUILD = ../../build

# set TRACE to '--trace' if VCD traces should be generated
# see the 'show' target below

SRCS = eco32test.v \
       src/eco32/eco32.v \
       src/clk_rst/clk_rst.v \
       src/cpu/cpu.v \
       src/cpu/cpu_bus.v \
       src/cpu/cpu_core.v \
       src/ram/ram.v \
       src/rom/rom.v \
       src/tmr/tmr.v \
       src/dsp/dsp.v \
       src/kbd/kbd.v \
       src/ser/ser.v
BIN = Veco32test

.PHONY:		all install run show clean

all:

install:

$(BIN):		$(SRCS) main.cpp
		verilator -Wall -Wno-style -Wno-CASEX -Wno-CASEOVERLAP \
		  $(TRACE) --cc $(SRCS) --exe main.cpp
		make -C obj_dir -j -f $(BIN).mk $(BIN)
		cp obj_dir/$(BIN) .

run:		$(BIN)
		@if [ ! -r duration.dat ] ; then \
		  echo "cp defdata/duration.dat ." ; \
		  cp defdata/duration.dat . ; \
		fi
		@if [ ! -r rom.dat ] ; then \
		  echo "cp defdata/rom.dat ." ; \
		  cp defdata/rom.dat . ; \
		fi
		@if [ ! -r kbd.dat ] ; then \
		  echo "cp defdata/kbd.dat ." ; \
		  cp defdata/kbd.dat . ; \
		fi
		@if [ ! -r ser0.dat ] ; then \
		  echo "cp defdata/ser0.dat ." ; \
		  cp defdata/ser0.dat . ; \
		fi
		@if [ ! -r ser1.dat ] ; then \
		  echo "cp defdata/ser1.dat ." ; \
		  cp defdata/ser1.dat . ; \
		fi
		./$(BIN)

show:
		$(MAKE) run TRACE='--trace'
		gtkwave dump.vcd eco32test.cfg

clean:
		rm -f *~ $(BIN) dump.vcd
		rm -f duration.dat rom.dat
		rm -f kbd.dat dsp.out
		rm -f ser0.dat ser0.out
		rm -f ser1.dat ser1.out
		rm -f src/*~
		rm -f src/eco32/*~ src/clk_rst/*~ src/cpu/*~
		rm -f src/ram/*~ src/rom/*~ src/tmr/*~
		rm -f src/dsp/*~ src/kbd/*~ src/ser/*~
		rm -rf obj_dir
