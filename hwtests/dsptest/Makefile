#
# Makefile for dsptest ROM
#

BUILD = ../../build

SRC = start.s main.c lib.c serial.s

.PHONY:		all install run clean

all:		dsptest.exo

install:	dsptest.exo

dsptest.exo:	dsptest.bin
		$(BUILD)/bin/bin2exo -S2 0 dsptest.bin dsptest.exo

dsptest.bin:	dsptest
		$(BUILD)/bin/load -p dsptest dsptest.bin

dsptest:	$(SRC)
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=romram.lnk \
		  -Wo-ldmap=dsptest.map -o dsptest $(SRC)

run:		dsptest.bin
		$(BUILD)/bin/sim -i -c -r dsptest.bin

clean:
		rm -f *~
		rm -f dsptest dsptest.map dsptest.bin dsptest.exo
