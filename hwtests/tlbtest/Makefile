#
# Makefile for tlbtest ROM
#

BUILD = ../../build

SRC = start.s main.c lib.c serial.s

.PHONY:		all install run clean

all:		tlbtest.exo

install:	tlbtest.exo

tlbtest.exo:	tlbtest.bin
		$(BUILD)/bin/bin2exo -S2 0 tlbtest.bin tlbtest.exo

tlbtest.bin:	tlbtest
		$(BUILD)/bin/load -p tlbtest tlbtest.bin

tlbtest:	$(SRC)
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=romram.lnk \
		  -Wo-ldmap=tlbtest.map -o tlbtest $(SRC)

run:		tlbtest.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r tlbtest.bin

clean:
		rm -f *~
		rm -f tlbtest tlbtest.map tlbtest.bin tlbtest.exo
