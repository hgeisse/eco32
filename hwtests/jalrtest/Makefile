#
# Makefile for jalrtest ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		jalrtest.exo

install:	jalrtest.exo

jalrtest.exo:	jalrtest.bin
		$(BUILD)/bin/bin2exo -S2 0 jalrtest.bin jalrtest.exo

jalrtest.bin:	jalrtest
		$(BUILD)/bin/load jalrtest jalrtest.bin

jalrtest:	jalrtest.o
		$(BUILD)/bin/ld -s rom.lnk -o jalrtest jalrtest.o

jalrtest.o:	jalrtest.s
		$(BUILD)/bin/as -o jalrtest.o jalrtest.s

run:		jalrtest.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r jalrtest.bin

clean:
		rm -f *~ jalrtest.o jalrtest jalrtest.bin jalrtest.exo
