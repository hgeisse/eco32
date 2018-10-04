#
# Makefile for icache test ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		ictest.exo

install:	ictest.exo

ictest.exo:	ictest.bin
		$(BUILD)/bin/bin2exo -S2 0 ictest.bin ictest.exo

ictest.bin:	ictest
		$(BUILD)/bin/load ictest ictest.bin

ictest:		ictest.o
		$(BUILD)/bin/ld -s rom.lnk -o ictest ictest.o

ictest.o:	ictest.s
		$(BUILD)/bin/as -o ictest.o ictest.s

run:		run-ica0

run-ica0:	ictest.bin
		$(BUILD)/bin/sim -ica 0 -i -s 1 -t 0 -r ictest.bin

run-ica1:	ictest.bin
		$(BUILD)/bin/sim -ica 1 -i -s 1 -t 0 -r ictest.bin

clean:
		rm -f *~ ictest.o ictest ictest.bin ictest.exo
