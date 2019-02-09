#
# Makefile for library test
#

BUILD = ../../../build

SRC = format.c
EXE = format
BIN = format.bin
EXE_LINUX = format-linux

all:		$(BIN) $(EXE_LINUX)

install:	$(BIN) $(EXE_LINUX)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN)

run-linux:	$(EXE_LINUX)
		./$(EXE_LINUX)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A -o $(EXE) $(SRC) -lm

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE_LINUX):	$(SRC)
		gcc -Wall -o $(EXE_LINUX) $(SRC) -lm

clean:
		rm -f *~ $(EXE) $(BIN) $(EXE_LINUX)
