#
# Makefile for application development
#

ECO32_BUILD = ../../../../../../build
BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld
LOAD = $(BUILD)/bin/load
SIM = $(ECO32_BUILD)/bin/sim
DOF = $(BUILD)/bin/dof

all:		bottles
		$(DOF) bottles

bottles:	start.o bottles.o
		$(LD) -c 0xC0000000 -o bottles \
		  start.o ../lib/libiolib.dso bottles.o

start.o:	start.s
		$(AS) -o start.o start.s

bottles.o:	bottles.c
		$(CC) $(CFLAGS) -I../include -o bottles.o -c bottles.c

clean:
		rm -f *~ start.o bottles.o bottles bottles.bin
