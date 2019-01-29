#
# Makefile for crt0 test
#

BUILD = ../../build

SRC = crt0test.c
EXE = crt0test
BIN = crt0test.bin

all:		$(BIN)

install:	$(BIN)
		mkdir -p $(BUILD)/libtest
		cp $(BIN) $(BUILD)/libtest

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A -o $(EXE) $(SRC)

clean:
		rm -f *~ $(EXE) $(BIN)
