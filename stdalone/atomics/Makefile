#
# Makefile for "atomics", a program to test the atomic instructions
#

BUILD = ../../build

SRC = start.s main.c end.s
BIN = atomics.bin
MAP = atomics.map
EXO = atomics.exo

.PHONY:		all install run clean

all:		$(BIN) $(EXO)

install:	$(BIN) $(EXO)
		mkdir -p $(BUILD)/stdalone
		cp $(BIN) $(BUILD)/stdalone
		cp $(MAP) $(BUILD)/stdalone
		cp $(EXO) $(BUILD)/stdalone

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN) -a 0x10000

run2:		$(BIN)
		/home/hellwig/summer-2016/summer-2016/eco32sim/eco32sim \
		  -i -s 1 -t 0 -l $(BIN) -a 0x10000

$(EXO):		$(BIN)
		$(BUILD)/bin/bin2exo -S2 0x10000 $(BIN) $(EXO)

$(BIN):		$(SRC)
		$(BUILD)/bin/lcc -A -Wo-kernel \
		  -Wl-m -Wl$(MAP) -o $(BIN) $(SRC)

clean:
		rm -f *~ $(BIN) $(MAP) $(EXO)
