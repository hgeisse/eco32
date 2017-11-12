#
# Makefile for ldtest ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		ldtest.exo

install:	ldtest.exo

ldtest.exo:	ldtest.bin
		$(BUILD)/bin/bin2exo -S2 0 ldtest.bin ldtest.exo

ldtest.bin:	ldtest
		$(BUILD)/bin/load ldtest ldtest.bin

ldtest:		ldtest.o
		$(BUILD)/bin/ld -s rom.lnk -o ldtest ldtest.o

ldtest.o:	ldtest.s
		$(BUILD)/bin/as -o ldtest.o ldtest.s

run:		ldtest.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r ldtest.bin

clean:
		rm -f *~ ldtest.o ldtest ldtest.bin ldtest.exo
