#
# Makefile for library test
#

BUILD = ../../../build

SRC = string.c
EXE = string
BIN = string.bin

all:		$(BIN)

install:	$(BIN)

run:		$(BIN)
		$(BUILD)/bin/sim -x -s 1 -t 0 -l $(BIN)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A -o $(EXE) $(SRC)

clean:
		rm -f *~ $(EXE) $(BIN)
