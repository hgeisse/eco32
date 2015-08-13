#
# Makefile for "hello"
#

BUILD = ../../../../build

SRC = start.s main.c end.s
BIN = hello.bin
MAP = hello.map

all:		$(BIN)

install:	$(BIN)

$(BIN):		$(SRC)
		$(BUILD)/bin/lcc -A -Wo-kernel \
		  -Wl-rc -Wl0xC0000000 -Wl-rd -Wl0xC000C000 \
		  -Wl-m -Wl$(MAP) -o $(BIN) $(SRC)

clean:
		rm -f *~ $(BIN) $(MAP)
