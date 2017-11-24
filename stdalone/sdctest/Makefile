#
# Makefile for "sdctest", a program for testing the SD card interface
#

BUILD = ../../build

SRC = start.s main.c iolib.c biolib.c
EXE = sdctest
BIN = sdctest.bin
MAP = sdctest.map
EXO = sdctest.exo

.PHONY:		all install run clean

all:		$(BIN) $(EXO)

install:	$(BIN) $(EXO)
		mkdir -p $(BUILD)/stdalone
		cp $(BIN) $(BUILD)/stdalone
		cp $(MAP) $(BUILD)/stdalone
		cp $(EXO) $(BUILD)/stdalone

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN) -a 0x10000 \
		  -D $(BUILD)/run/disk.img

$(EXO):		$(BIN)
		$(BUILD)/bin/bin2exo -S2 0x10000 $(BIN) $(EXO)

$(BIN):		$(EXE)
		$(BUILD)/bin/load -p $(EXE) $(BIN)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=stdalone.lnk \
		  -Wo-ldmap=$(MAP) -o $(EXE) $(SRC)

clean:
		rm -f *~ $(EXE) $(BIN) $(MAP) $(EXO)
