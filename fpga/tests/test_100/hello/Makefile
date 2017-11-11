#
# Makefile for "hello"
#

BUILD = ../../../../build

SRC = start.s main.c
EXE = hello
BIN = hello.bin
MAP = hello.map

all:		$(BIN)

install:	$(BIN)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=standalone.lnk \
		  -Wo-ldmap=$(MAP) -o $(EXE) $(SRC)

clean:
		rm -f *~ $(EXE) $(BIN) $(MAP)
