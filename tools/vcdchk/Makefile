#
# Makefile for VCD check utility
#

BUILD = ../../build

all:		vcdchk

install:	vcdchk
		mkdir -p $(BUILD)/bin
		cp vcdchk $(BUILD)/bin

vcdchk:		vcdchk.c
		gcc -g -Wall -o vcdchk vcdchk.c

clean:
		rm -f *~ vcdchk
