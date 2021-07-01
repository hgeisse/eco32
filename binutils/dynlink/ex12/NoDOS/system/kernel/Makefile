#
# Makefile for NoDOS kernel
#

ECO32_BUILD = ../../../../../../build
BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld
LOAD = $(BUILD)/bin/load
SIM = $(ECO32_BUILD)/bin/sim

KMODS = start.o main.o iolib.o biolib.o disk.o

all:		nodos

run:		nodos.bin
		$(SIM) -i -l nodos.bin -s 1 -t 0

nodos.bin:	nodos
		$(LOAD) -v nodos nodos.bin

nodos:		$(KMODS)
		$(LD) -c 0xC0000000 -o nodos $(KMODS)

start.o:	start.s
		$(AS) -o start.o start.s

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

clean:
		rm -f *~ *.o nodos nodos.bin
