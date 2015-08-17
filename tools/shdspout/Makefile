#
# Makefile for "shdspout" tool
#

BUILD = ../../build

.PHONY:		all install clean

all:		shdspout

install:	shdspout
		mkdir -p $(BUILD)/bin
		cp shdspout $(BUILD)/bin

shdspout:	shdspout.c
		gcc -g -Wall -o shdspout shdspout.c -lncurses

clean:
		rm -f *~ shdspout
