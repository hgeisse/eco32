#
# Makefile for building a hosted executable
#

BUILD = ../../../../../build
CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld

all:		mini

mini:		start.o mini.o
		$(LD) -c 0xC0100000 -o mini \
		  start.o mini.o

start.o:	start.s
		$(AS) -o start.o start.s

mini.o:		mini.c
		$(CC) $(CFLAGS) -I../include -o mini.o -c mini.c

clean:
		rm -f *~ start.o mini.o mini
