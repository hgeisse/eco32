#
# Makefile for memory speedometer on XESS XSA-XST-3 board
#

BUILD = ../../../build

.PHONY:		all install clean

all:		build/memspeed.bit

install:	build/memspeed.bit

clean:
		mv build/memspeed.xise .
		mv build/memspeed.bit .
		rm -rf build/*
		mv memspeed.xise build
		mv memspeed.bit build
		rm -f *~
