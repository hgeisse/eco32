#
# Makefile for building a hosted executable
#

BUILD = ../../../../../build
CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld

all:		ls

ls:		start.o ls.o
		$(LD) -c 0xC0100000 -o ls \
		  start.o ls.o

start.o:	start.s
		$(AS) -o start.o start.s

ls.o:		ls.c
		$(CC) $(CFLAGS) -I../include -o ls.o -c ls.c

clean:
		rm -f *~ start.o ls.o ls
