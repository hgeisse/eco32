#
# Makefile to build the partition boot record
#

BUILD = ../../../../build

all:		pbr.bin

pbr.bin:	pbr.o
		$(BUILD)/bin/ld -h -rc 0xC0010000 -o pbr.bin pbr.o

pbr.o:		pbr.s
		$(BUILD)/bin/as -o pbr.o pbr.s

clean:
		rm -f *~ pbr.o pbr.bin
