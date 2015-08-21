#
# Makefile for memory test on XESS XSA-XST-3 board
#

BUILD = ../../../../../build

.PHONY:		all install clean

all:		build/memtest.bit

install:	build/memtest.bit

clean:
		mv build/memtest.xise .
		mv build/memtest.bit .
		rm -rf build/*
		mv memtest.xise build
		mv memtest.bit build
		rm -f *~
