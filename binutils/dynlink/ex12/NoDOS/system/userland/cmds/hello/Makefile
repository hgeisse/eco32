#
# Makefile for building a hosted executable
#

BUILD = ../../../../../build
CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld

all:		hello

hello:		start.o hello.o
		$(LD) -c 0xC0100000 -o hello \
		  start.o hello.o

start.o:	start.s
		$(AS) -o start.o start.s

hello.o:	hello.c
		$(CC) $(CFLAGS) -I../include -o hello.o -c hello.c

clean:
		rm -f *~ start.o hello.o hello
