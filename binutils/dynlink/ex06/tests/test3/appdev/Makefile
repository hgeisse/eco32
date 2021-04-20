#
# Makefile for application development
#

BUILD = ../../../../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A
AS = $(BUILD)/bin/as
LOAD = $(BUILD)/bin/load
SIM = $(BUILD)/bin/sim

all:		app

run:		app.bin
		$(SIM) -i -l app.bin -a 0x1000 -s 1 -t 0

app.bin:	app
		$(LOAD) app app.bin

app:		start.o app.o
		../../../ld/ld -c 0xc0001000 -o app \
		  start.o app.o ../lib/libmylib.a

start.o:	start.s
		$(AS) -o start.o start.s

app.o:		app.c
		$(CC) $(CFLAGS) -I../include -o app.o -c app.c

clean:
		rm -f *~ start.o app.o app app.bin
