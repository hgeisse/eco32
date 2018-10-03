#
# Makefile for address pattern generator
#

BUILD = ../../build

all:		genaddr

install:	genaddr
		mkdir -p $(BUILD)/bin
		cp genaddr $(BUILD)/bin

genaddr:	genaddr.c
		gcc -g -Wall -o genaddr genaddr.c

clean:
		rm -f *~ genaddr
