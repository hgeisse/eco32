#
# Makefile to build the bootstrap (boot loader)
#

BUILD = ../../../../../build

all:		boot.bin

boot.bin:	c0.o boot.o biolib.o c1.o
		$(BUILD)/bin/ld -h -rc 0xC0011000 -o boot.bin \
		  c0.o boot.o biolib.o c1.o

c0.o:		c0.s
		$(BUILD)/bin/as -o c0.o c0.s

boot.o:		boot.c
		$(BUILD)/bin/lcc -A -c -o boot.o boot.c

biolib.o:	biolib.s
		$(BUILD)/bin/as -o biolib.o biolib.s

c1.o:		c1.s
		$(BUILD)/bin/as -o c1.o c1.s

clean:
		rm -f *~ c0.o boot.o biolib.o c1.o boot.bin
