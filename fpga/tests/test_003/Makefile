#
# Makefile to build a test
#

BUILD = ../../../build

ROM_SIZE_KB = 64

SRC = $(wildcard *.s)
OBJ = $(patsubst %.s,%.o,$(SRC))
BIN = $(patsubst %.s,%.bin,$(SRC))
DAT = $(patsubst %.s,%.dat,$(SRC))

all:		$(DAT)

install:	$(DAT)

$(DAT):		$(BIN)
		$(BUILD)/bin/bin2dat $(BIN) $(DAT) $(ROM_SIZE_KB)

$(BIN):		$(OBJ)
		$(BUILD)/bin/ld -s rom.lnk -o tmp $(OBJ)
		$(BUILD)/bin/load tmp $(BIN)
		rm -f tmp

$(OBJ):		$(SRC)
		$(BUILD)/bin/as -o $(OBJ) $(SRC)

clean:
		rm -f *~ $(OBJ) $(BIN) $(DAT)
