#
# Makefile for building a hosted executable
#

BUILD = ../../../../../build
CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld

all:		cat

cat:		start.o cat.o
		$(LD) -c 0xC0100000 -o cat \
		  start.o cat.o

start.o:	start.s
		$(AS) -o start.o start.s

cat.o:		cat.c
		$(CC) $(CFLAGS) -I../include -o cat.o -c cat.c

clean:
		rm -f *~ start.o cat.o cat
