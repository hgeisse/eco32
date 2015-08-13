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
		$(BUILD)/bin/ld -h -rc 0xC0000000 -o $(BIN) $(OBJ)

$(OBJ):		$(SRC)
		$(BUILD)/bin/as -o $(OBJ) $(SRC)

clean:
		rm -f *~ $(OBJ) $(BIN)
