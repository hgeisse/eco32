#
# Makefile for building a hosted executable
#

BUILD = ../../../../build
CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld

all:		halt

halt:		start.o halt.o
		$(LD) -c 0xC0100000 -o halt \
		  start.o halt.o

start.o:	start.s
		$(AS) -o start.o start.s

halt.o:		halt.c
		$(CC) $(CFLAGS) -I../include -o halt.o -c halt.c

clean:
		rm -f *~ start.o halt.o halt
