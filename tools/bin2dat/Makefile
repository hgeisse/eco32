#
# Makefile for binary to simulation data converter
#

BUILD = ../../build

.PHONY:		all install clean

all:		bin2dat

install:	bin2dat
		mkdir -p $(BUILD)/bin
		cp bin2dat $(BUILD)/bin

bin2dat:	bin2dat.c
		gcc -g -Wall -o bin2dat bin2dat.c

clean:
		rm -f *~ bin2dat
