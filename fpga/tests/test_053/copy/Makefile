#
# Makefile for ROM-to-RAM copy program
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		copy.bin

install:	copy.bin

copy.bin:	copy.o size.o
		$(BUILD)/bin/ld -o copy.bin -m copy.map \
		  -h -rc 0xE0000000 copy.o size.o

copy.o:		copy.s
		$(BUILD)/bin/as -o copy.o copy.s

size.o:		../size.s
		$(BUILD)/bin/as -o size.o ../size.s

clean:
		rm -f *~ copy.o size.o copy.bin copy.map
