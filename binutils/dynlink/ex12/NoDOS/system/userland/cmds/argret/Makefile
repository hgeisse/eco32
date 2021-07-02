#
# Makefile for building a hosted executable
#

BUILD = ../../../../../build
CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld

all:		argret

argret:		start.o argret.o
		$(LD) -c 0xC0100000 -o argret \
		  start.o argret.o

start.o:	start.s
		$(AS) -o start.o start.s

argret.o:	argret.c
		$(CC) $(CFLAGS) -I../include -o argret.o -c argret.c

clean:
		rm -f *~ start.o argret.o argret
