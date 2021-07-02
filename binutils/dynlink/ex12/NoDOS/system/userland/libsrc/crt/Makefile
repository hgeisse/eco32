#
# Makefile for startup
#

BUILD = ../../../../../build
LIB = ../../lib

AS = $(BUILD)/bin/as

all:		crt0.o
		mkdir -p $(LIB)
		cp crt0.o $(LIB)

crt0.o:		crt0.s
		$(AS) -o crt0.o crt0.s

clean:
		rm -f *~ crt0.o
