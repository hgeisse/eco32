#
# Makefile for ROM-to-RAM copy program
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		copy.bin

install:	copy.bin

copy.bin:	copy
		$(BUILD)/bin/load copy copy.bin

copy:		copy.o size.o
		$(BUILD)/bin/ld -o copy -m copy.map \
		  -s rom.lnk copy.o size.o

copy.o:		copy.s
		$(BUILD)/bin/as -o copy.o copy.s

size.o:		../size.s
		$(BUILD)/bin/as -o size.o ../size.s

clean:
		rm -f *~ copy.o size.o copy copy.bin copy.map
