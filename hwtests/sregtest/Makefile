#
# Makefile for sregtest ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		sregtest.exo

install:	sregtest.exo

sregtest.exo:	sregtest.bin
		$(BUILD)/bin/bin2exo -S2 0 sregtest.bin sregtest.exo

sregtest.bin:	sregtest
		$(BUILD)/bin/load sregtest sregtest.bin

sregtest:	sregtest.o
		$(BUILD)/bin/ld -s rom.lnk -o sregtest sregtest.o

sregtest.o:	sregtest.s
		$(BUILD)/bin/as -o sregtest.o sregtest.s

run:		sregtest.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r sregtest.bin

clean:
		rm -f *~ sregtest.o sregtest sregtest.bin sregtest.exo
