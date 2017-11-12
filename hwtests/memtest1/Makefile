#
# Makefile for memtest1 ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		memtest1.exo

install:	memtest1.exo

memtest1.exo:	memtest1.bin
		$(BUILD)/bin/bin2exo -S2 0 memtest1.bin memtest1.exo

memtest1.bin:	memtest1
		$(BUILD)/bin/load memtest1 memtest1.bin

memtest1:	memtest1.o
		$(BUILD)/bin/ld -s rom.lnk -o memtest1 memtest1.o

memtest1.o:	memtest1.s
		$(BUILD)/bin/as -o memtest1.o memtest1.s

run:		memtest1.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r memtest1.bin

clean:
		rm -f *~ memtest1.o memtest1 memtest1.bin memtest1.exo
