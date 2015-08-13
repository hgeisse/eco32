#
# Makefile for "dhrystone", the well-known integer benchmark
#

BUILD = ../../../../build

SRC = port/start.s dhry_1.c dhry_2.c port/dhry_utils.c \
      port/iolib.c port/biolib.c port/end.s
BIN = dhry.bin
MAP = dhry.map

all:		$(BIN)

install:	$(BIN)

$(BIN):		$(SRC)
		$(BUILD)/bin/lcc -A -Wo-kernel -Wl-m -Wl$(MAP) \
		  -Wl-rc -Wl0xC0000000 -Wl-rd -Wl0xC000C000 \
		  -o $(BIN) -DNOFLOAT -DSTANDALONE -DRUNS=1000 \
		  -I. -Iport $(SRC)

clean:
		rm -f port/*~ *~ $(BIN) $(MAP)
