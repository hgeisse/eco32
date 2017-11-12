#
# Makefile for shtest ROM
#

BUILD = ../../build

.PHONY:		all install run clean

all:		shtest.exo

install:	shtest.exo

shtest.exo:	shtest.bin
		$(BUILD)/bin/bin2exo -S2 0 shtest.bin shtest.exo

shtest.bin:	shtest
		$(BUILD)/bin/load shtest shtest.bin

shtest:		shtest.o
		$(BUILD)/bin/ld -s rom.lnk -o shtest shtest.o

shtest.o:	shtest.s
		$(BUILD)/bin/as -o shtest.o shtest.s

shtest.s:	mkshtest
		./mkshtest >shtest.s

mkshtest:	mkshtest.c
		gcc -g -Wall -o mkshtest mkshtest.c

run:		shtest.bin
		$(BUILD)/bin/sim -i -s 1 -t 0 -r shtest.bin

clean:
		rm -f *~ mkshtest shtest.s shtest.o
		rm -f shtest shtest.bin shtest.exo
