#
# Makefile for dcache test ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		dctest.exo

install:	dctest.exo

dctest.exo:	dctest.bin
		$(BUILD)/bin/bin2exo -S2 0 dctest.bin dctest.exo

dctest.bin:	dctest
		$(BUILD)/bin/load dctest dctest.bin

dctest:		dctest.o
		$(BUILD)/bin/ld -s rom.lnk -o dctest dctest.o

dctest.o:	dctest.s
		$(BUILD)/bin/as -o dctest.o dctest.s

run:		run-dca0

run-dca0:	dctest.bin
		$(BUILD)/bin/sim -dca 0 -i -s 1 -t 0 -r dctest.bin

run-dca1:	dctest.bin
		$(BUILD)/bin/sim -dca 1 -i -s 1 -t 0 -r dctest.bin

clean:
		rm -f *~ dctest.o dctest dctest.bin dctest.exo
