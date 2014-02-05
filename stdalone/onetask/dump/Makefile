#
# Makefile for dump utility
#

.PHONY:		all install clean

all:		dump

install:	dump

dump:		dump.c
		gcc -m32 -g -Wall -o dump dump.c

clean:
		rm -f *~ dump
