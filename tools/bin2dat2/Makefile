#
# Makefile for binary to simulation data converter
# (produce two 16-bit files)
#

BUILD = ../../build

.PHONY:		all install clean

all:		bin2dat2

install:	bin2dat2
		mkdir -p $(BUILD)/bin
		cp bin2dat2 $(BUILD)/bin

bin2dat2:	bin2dat2.c
		gcc -g -Wall -o bin2dat2 bin2dat2.c

clean:
		rm -f *~ bin2dat2
