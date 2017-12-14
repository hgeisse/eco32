#
# Makefile for copybig ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		copybig.exo

install:	copybig.exo

copybig.exo:	copybig.bin
		$(BUILD)/bin/bin2exo -S2 0 copybig.bin copybig.exo

copybig.bin:	copybig
		$(BUILD)/bin/load copybig copybig.bin

copybig:	copybig.o
		$(BUILD)/bin/ld -s rom.lnk -o copybig copybig.o

copybig.o:	copybig.s
		$(BUILD)/bin/as -o copybig.o copybig.s

copybig.s:	copy.s data.s
		cat copy.s data.s >copybig.s

data.s:		mkdata
		./mkdata >data.s

mkdata:		mkdata.c
		gcc -g -Wall -o mkdata mkdata.c

run:		copybig.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r copybig.bin

clean:
		rm -f *~ mkdata data.s copybig.s copybig.o
		rm -f copybig copybig.bin copybig.exo
