#
# Makefile for simple test
#

BUILD = ../../../../build

all:		simple

simple:		start.o main.o func.o
		../ld -m simple.map -o simple start.o main.o func.o

start.o:	start.s
		$(BUILD)/bin/lcc -A -o start.o -c start.s

%.o:		%.c
		$(BUILD)/bin/lcc -A -o $@ -c $<

clean:
		rm -f *~ *.o simple simple.bin simple.map
