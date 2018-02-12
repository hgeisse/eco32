#
# Makefile for simple test
#

BUILD = ../../../build

all:		simple.bin

run:		simple.bin
		$(BUILD)/bin/sim -i -l simple.bin

simple.bin:	simple
		$(BUILD)/bin/load simple simple.bin

simple:		main.o func.o
		$(BUILD)/bin/lcc -A -Wo-ldmap=simple.map \
		  -o simple main.o func.o

%.o:		%.c
		$(BUILD)/bin/lcc -A -o $@ -c $<

clean:
		rm -f *~ *.o simple simple.bin simple.map
