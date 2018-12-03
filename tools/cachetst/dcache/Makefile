#
# Makefile for dcache test
#

BUILD = ../../../build

.PHONY:		all install clean

all:		dctest

install:	dctest
		mkdir -p $(BUILD)/bin
		cp dctest $(BUILD)/bin

dctest:		dctest.c
		gcc -g -Wall -o dctest dctest.c

clean:
		rm -f *~ dctest
