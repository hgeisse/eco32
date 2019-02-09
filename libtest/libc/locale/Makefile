#
# Makefile for library test
#

BUILD = ../../../build

SRC = locale.c
EXE = locale
BIN = locale.bin

all:		$(BIN)

install:	$(BIN)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A -o $(EXE) $(SRC)

clean:
		rm -f *~ $(EXE) $(BIN)
