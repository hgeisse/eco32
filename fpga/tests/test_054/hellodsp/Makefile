#
# Makefile for "hellodsp"
#

BUILD = ../../../../build

SRC = hellodsp.s
OBJ = hellodsp.o
BIN = hellodsp.bin

all:		$(BIN)

install:	$(BIN)

run:		$(BIN)
		$(BUILD)/bin/sim -i -c -l $(BIN)

$(BIN):		$(OBJ)
		$(BUILD)/bin/ld -h -rc 0xC0000000 -o $(BIN) $(OBJ)

$(OBJ):		$(SRC)
		$(BUILD)/bin/as -o $(OBJ) $(SRC)

clean:
		rm -f *~ $(OBJ) $(BIN)
