#
# Makefile for application development
#

ECO32_BUILD = ../../../../../../build
BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-pic
AS = $(BUILD)/bin/as
LD = $(BUILD)/bin/ld
LOAD = $(BUILD)/bin/load
SIM = $(ECO32_BUILD)/bin/sim

all:		main

run:		main.bin
		$(SIM) -i -l main.bin -s 1 -t 0

main.bin:	main
		$(LOAD) -v -d ../lib main main.bin

main:		start.o main.o
		$(LD) -pic -c 0xC0000000 -o main \
		  start.o main.o ../lib/liblib1.dso

start.o:	start.s
		$(AS) -o start.o start.s

main.o:		main.c
		$(CC) $(CFLAGS) -I../include -o main.o -c main.c

clean:
		rm -f *~ start.o main.o main main.bin
