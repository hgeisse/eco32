#
# Makefile for "shpart", a program to show the partitions on a hard disk
#

BUILD = ../../build

SRC = start.s main.c iolib.c biolib.c end.s
BIN = shpart.bin
MAP = shpart.map

.PHONY:		all install run clean

all:		$(BIN)

install:	$(BIN)
		mkdir -p $(BUILD)/stdalone
		cp $(BIN) $(BUILD)/stdalone
		cp $(MAP) $(BUILD)/stdalone

run:		$(BIN)
		$(BUILD)/bin/sim -i -t 1 -l $(BIN) -d $(BUILD)/disk/disk.img

$(BIN):		$(SRC)
		$(BUILD)/bin/lcc -A -Wo-kernel \
		  -Wl-m -Wl$(MAP) -o $(BIN) $(SRC)

clean:
		rm -f *~ $(BIN) $(MAP)
