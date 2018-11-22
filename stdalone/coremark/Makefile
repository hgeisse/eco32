#
# Makefile for "coremark", an alternative integer benchmark
#

BUILD = ../../build

EXE = coremark
BIN = coremark.bin
MAP = coremark.map
EXO = coremark.exo

.PHONY:		all install run clean

all:		$(BIN) $(EXO)

install:	$(BIN) $(EXO)
		mkdir -p $(BUILD)/stdalone
		cp $(BIN) $(BUILD)/stdalone
		cp $(MAP) $(BUILD)/stdalone
		cp $(EXO) $(BUILD)/stdalone

run:		$(BIN)
		$(BUILD)/bin/sim -i -m 32 -s 1 -t 0 -l $(BIN) -a 0x10000

$(EXO):		$(BIN)
		$(BUILD)/bin/bin2exo -S2 0x10000 $(BIN) $(EXO)

$(BIN):		$(EXE)
		$(BUILD)/bin/load -p $(EXE) $(BIN)

$(EXE):
		$(MAKE) -C coremark-src PORT_DIR=eco32
		cp coremark-src/coremark .
		cp coremark-src/coremark.map .

check:
		$(MAKE) -C coremark-src PORT_DIR=eco32 check

clean:
		$(MAKE) -C coremark-src PORT_DIR=eco32 clean
		rm -f *~ $(EXE) $(BIN) $(MAP) $(EXO)
