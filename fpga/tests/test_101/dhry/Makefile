#
# Makefile for "dhrystone", the well-known integer benchmark
#

BUILD = ../../../../build

SRC = port/start.s dhry_1.c dhry_2.c port/dhry_utils.c \
      port/iolib.c port/biolib.c
EXE = dhry
BIN = dhry.bin
MAP = dhry.map

all:		$(BIN)

install:	$(BIN)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE):		$(SRC)
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=port/standalone.lnk \
		  -Wo-ldmap=$(MAP) -o $(EXE) \
		  -DNOFLOAT -DSTANDALONE -DRUNS=1000 \
		  -I. -Iport $(SRC)

clean:
		rm -f port/*~ *~ $(EXE) $(BIN) $(MAP)
