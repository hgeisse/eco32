#
# Makefile for library test
#

BUILD = ../../../build

SRC = stdarg.c
EXE = stdarg
BIN = stdarg.bin

all:		$(BIN)

install:	$(BIN)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A -o $(EXE) $(SRC)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

clean:
		rm -f *~ $(EXE) $(BIN)
