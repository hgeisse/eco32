#
# Makefile for library test
#

BUILD = ../../../build

SRC = float.c
EXE = float
BIN = float.bin
EXE_LINUX = float-linux

all:		$(BIN) $(EXE_LINUX)

install:	$(BIN) $(EXE_LINUX)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN)

run-linux:	$(EXE_LINUX)
		./$(EXE_LINUX)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A -o $(EXE) $(SRC)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE_LINUX):	$(SRC)
		gcc -Wall -o $(EXE_LINUX) $(SRC)

clean:
		rm -f *~ $(EXE) $(BIN) $(EXE_LINUX)
