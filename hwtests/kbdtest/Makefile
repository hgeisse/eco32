#
# Makefile for kbdtest ROM
#

BUILD = ../../build

SRC = start.s main.c lib.c serial.s

.PHONY:		all install run clean

all:		kbdtest.exo

install:	kbdtest.exo

kbdtest.exo:	kbdtest.bin
		$(BUILD)/bin/bin2exo -S2 0 kbdtest.bin kbdtest.exo

kbdtest.bin:	kbdtest
		$(BUILD)/bin/load -p kbdtest kbdtest.bin

kbdtest:	$(SRC)
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=romram.lnk \
		  -Wo-ldmap=kbdtest.map -o kbdtest $(SRC)

run:		kbdtest.bin
		$(BUILD)/bin/sim -i -c -s 1 -t 0 -r kbdtest.bin

clean:
		rm -f *~
		rm -f kbdtest kbdtest.map kbdtest.bin kbdtest.exo
