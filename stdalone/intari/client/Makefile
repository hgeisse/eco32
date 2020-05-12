#
# Makefile for integer arithmetic test, client
#

BUILD = ../../../build

.PHONY:		all install clean

all:		iatest

install:	iatest
		cp iatest $(BUILD)/stdalone

iatest:		iatest.c
		gcc -g -Wall -o iatest iatest.c

clean:
		rm -f *~ iatest
