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
		$(BUILD)/bin/ld -s ram.lnk -o tmp $(OBJ)
		$(BUILD)/bin/load tmp $(BIN)
		rm -f tmp

$(OBJ):		$(SRC)
		$(BUILD)/bin/as -o $(OBJ) $(SRC)

clean:
		rm -f *~ $(OBJ) $(BIN)
