#
# Makefile to build the master boot record
#

BUILD = ../../../../build

all:		mbr.bin

mbr.bin:	mbr
		$(BUILD)/bin/load -p mbr mbr.bin

mbr:		mbr.o
		$(BUILD)/bin/ld -s stage1.lnk -o mbr mbr.o

mbr.o:		mbr.s
		$(BUILD)/bin/as -o mbr.o mbr.s

clean:
		rm -f *~ mbr.o mbr mbr.bin
