#
# Makefile for library test
#

BUILD = ../../build

SRC = abi.c
ASM = abi.s
EXE = abi
BIN = abi.bin
EXE_LINUX = abi-linux

all:		$(ASM) $(BIN) $(EXE_LINUX)

install:	$(ASM) $(BIN) $(EXE_LINUX)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN)

run-linux:	$(EXE_LINUX)
		./$(EXE_LINUX)

$(ASM):		$(SRC)
		$(BUILD)/bin/lcc -A -S -o $(ASM) $(SRC)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A -o $(EXE) $(SRC)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE_LINUX):	$(SRC)
		gcc -Wall -o $(EXE_LINUX) $(SRC)

clean:
		rm -f *~ $(ASM) $(EXE) $(BIN) $(EXE_LINUX)
