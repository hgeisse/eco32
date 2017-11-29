#
# Makefile for memtest3 ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		memtest3.exo

install:	memtest3.exo

memtest3.exo:	memtest3.bin
		$(BUILD)/bin/bin2exo -S2 0 memtest3.bin memtest3.exo

memtest3.bin:	memtest3
		$(BUILD)/bin/load memtest3 memtest3.bin

memtest3:	memtest3.o
		$(BUILD)/bin/ld -s rom.lnk -o memtest3 memtest3.o

memtest3.o:	memtest3.s
		$(BUILD)/bin/as -o memtest3.o memtest3.s

run:		memtest3.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r memtest3.bin

clean:
		rm -f *~ memtest3.o memtest3 memtest3.bin memtest3.exo
