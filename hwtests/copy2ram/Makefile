#
# Makefile for copy2ram ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		copy2ram.exo

install:	copy2ram.exo

copy2ram.exo:	copy2ram.bin
		$(BUILD)/bin/bin2exo -S2 0 copy2ram.bin copy2ram.exo

copy2ram.bin:	copy2ram
		$(BUILD)/bin/load copy2ram copy2ram.bin

copy2ram:	copy2ram.o
		$(BUILD)/bin/ld -s rom.lnk -o copy2ram copy2ram.o

copy2ram.o:	copy2ram.s
		$(BUILD)/bin/as -o copy2ram.o copy2ram.s

run:		copy2ram.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r copy2ram.bin

clean:
		rm -f *~ copy2ram.o copy2ram copy2ram.bin copy2ram.exo
