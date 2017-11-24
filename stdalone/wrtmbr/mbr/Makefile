#
# Makefile for assembling the default master boot record
#

BUILD = ../../../build

.PHONY:		all clean

all:		mbr.dump

mbr.dump:	dump mbr.bin
		./dump mbr.bin mbr.dump

dump:		dump.c
		gcc -g -Wall -o dump dump.c

mbr.bin:	mbr
		$(BUILD)/bin/load -p mbr mbr.bin

mbr:		mbr.o
		$(BUILD)/bin/ld -s stage1.lnk -o mbr mbr.o

mbr.o:		mbr.s
		$(BUILD)/bin/as -o mbr.o mbr.s

clean:
		rm -f *~ dump mbr.o mbr mbr.bin mbr.dump
