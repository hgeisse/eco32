#
# Makefile for packet exchange device test, client side
#

BUILD = ../../../build

.PHONY:		all install clean

all:		pxdtest

install:	pxdtest
		mkdir -p $(BUILD)/stdalone
		cp pxdtest $(BUILD)/stdalone

pxdtest:	pxdtest.c
		gcc -g -Wall -o pxdtest pxdtest.c

clean:
		rm -f *~ pxdtest
