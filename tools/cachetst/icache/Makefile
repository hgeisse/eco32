#
# Makefile for icache test
#

BUILD = ../../../build

.PHONY:		all install clean

all:		ictest

install:	ictest
		mkdir -p $(BUILD)/bin
		cp ictest $(BUILD)/bin

ictest:		ictest.c
		gcc -g -Wall -o ictest ictest.c

clean:
		rm -f *~ ictest
