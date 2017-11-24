#
# Makefile to build the master bootstrap (boot manager)
#

BUILD = ../../../../build

all:		mboot.bin

mboot.bin:	mboot
		$(BUILD)/bin/load -p mboot mboot.bin

mboot:		c0.o mboot.o biolib.o
		$(BUILD)/bin/ld -s stage2.lnk -o mboot \
		  c0.o mboot.o biolib.o

c0.o:		c0.s
		$(BUILD)/bin/as -o c0.o c0.s

mboot.o:	mboot.c
		$(BUILD)/bin/lcc -A -c -o mboot.o mboot.c

biolib.o:	biolib.s
		$(BUILD)/bin/as -o biolib.o biolib.s

clean:
		rm -f *~ c0.o mboot.o biolib.o mboot mboot.bin
