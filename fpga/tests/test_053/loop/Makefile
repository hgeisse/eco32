#
# Makefile for "loop"
#

BUILD = ../../../../build

SRC = loop.s
OBJ = loop.o
BIN = loop.bin

all:		$(BIN)

install:	$(BIN)

run:		$(BIN)
		$(BUILD)/bin/sim -i -l $(BIN)

$(BIN):		$(OBJ)
		$(BUILD)/bin/ld -s ram.lnk -o tmp $(OBJ)
		$(BUILD)/bin/load tmp $(BIN)
		rm -f tmp

$(OBJ):		$(SRC)
		$(BUILD)/bin/as -o $(OBJ) $(SRC)

clean:
		rm -f *~ $(OBJ) $(BIN)
