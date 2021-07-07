#
# Makefile for application development
#

ECO32_BUILD = ../../../../../../build
BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-pic
AS = $(BUILD)/bin/as -pic
LD = $(BUILD)/bin/ld
LOAD = $(BUILD)/bin/load
SIM = $(ECO32_BUILD)/bin/sim

all:		bottles

runlo:		bottleslo.bin
		$(SIM) -i -l bottleslo.bin -s 1 -t 0

bottleslo.bin:	bottles
		$(LOAD) -v bottles bottleslo.bin

runhi:		bottleshi.bin
		$(SIM) -i -l bottleshi.bin -a 0x3000 -s 1 -t 0

bottleshi.bin:	bottles
		$(LOAD) -v -l 0x3000 bottles bottleshi.bin

bottles:	start.o bottles.o
		$(LD) -pic -c 0xC0000000 -o bottles \
		  start.o bottles.o ../lib/libiolib.a ../lib/libbiolib.a

start.o:	start.s
		$(AS) -o start.o start.s

bottles.o:	bottles.c
		$(CC) $(CFLAGS) -I../include -o bottles.o -c bottles.c

clean:
		rm -f *~ start.o bottles.o bottles
		rm -f bottleslo.bin bottleshi.bin
