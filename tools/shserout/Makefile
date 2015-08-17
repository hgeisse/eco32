#
# Makefile for "shserout" tool
#

BUILD = ../../build

.PHONY:		all install clean

all:		shserout

install:	shserout
		mkdir -p $(BUILD)/bin
		cp shserout $(BUILD)/bin

shserout:	shserout.c
		gcc -g -Wall -o shserout shserout.c

clean:
		rm -f *~ shserout
