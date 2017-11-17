#
# Makefile for ROM-to-RAM copy program
#

BUILD = ../../../build

.PHONY:		all install clean

all:		copy.bin

install:	copy.bin

copy.bin:	copy
		$(BUILD)/bin/load copy copy.bin

copy:		copy.o
		$(BUILD)/bin/ld -o copy -m copy.map \
		  -s rom.lnk copy.o

copy.o:		copy.s
		$(BUILD)/bin/as -o copy.o copy.s

clean:
		rm -f *~ copy.o copy copy.bin copy.map
