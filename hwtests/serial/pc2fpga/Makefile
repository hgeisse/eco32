#
# Makefile for serial line test program (PC --> FPGA)
#

BUILD = ../../../build

.PHONY:		all install clean

all:		receive0.exo receive1.exo send

install:	receive0.exo receive1.exo send

receive0.o:	receive0.s
		$(BUILD)/bin/as -o receive0.o receive0.s

receive1.o:	receive1.s
		$(BUILD)/bin/as -o receive1.o receive1.s

receive0:	receive0.o
		$(BUILD)/bin/ld -s rom.lnk -o receive0 receive0.o

receive1:	receive1.o
		$(BUILD)/bin/ld -s rom.lnk -o receive1 receive1.o

receive0.bin:	receive0
		$(BUILD)/bin/load receive0 receive0.bin

receive1.bin:	receive1
		$(BUILD)/bin/load receive1 receive1.bin

receive0.exo:	receive0.bin
		$(BUILD)/bin/bin2exo -S2 0 receive0.bin receive0.exo

receive1.exo:	receive1.bin
		$(BUILD)/bin/bin2exo -S2 0 receive1.bin receive1.exo

send:		send.c
		gcc -g -Wall -o send send.c

clean:
		rm -f *~
		rm -f receive0.o receive0 receive0.bin receive0.exo
		rm -f receive1.o receive1 receive1.bin receive1.exo
		rm -f send
