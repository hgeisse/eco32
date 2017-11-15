#
# Makefile for xcptest ROM
#

BUILD = ../../build

SRC = start.s main.c lib.c serial.s

.PHONY:		all install run clean

all:		xcptest.exo

install:	xcptest.exo

xcptest.exo:	xcptest.bin
		$(BUILD)/bin/bin2exo -S2 0 xcptest.bin xcptest.exo

xcptest.bin:	xcptest
		$(BUILD)/bin/load -p xcptest xcptest.bin

xcptest:	$(SRC)
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=romram.lnk \
		  -Wo-ldmap=xcptest.map -o xcptest $(SRC)

run:		xcptest.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r xcptest.bin

clean:
		rm -f *~
		rm -f xcptest xcptest.map xcptest.bin xcptest.exo
