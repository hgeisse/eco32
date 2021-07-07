#
# Makefile for library development
#

BUILD = ../../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-pic -I../biolib
LDFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-shared
LDLIBS = ../biolib/libbiolib.dso

all:		types.h stdarg.h iolib.h libiolib.dso
		mkdir -p ../../include
		cp types.h ../../include
		cp stdarg.h ../../include
		cp iolib.h ../../include
		mkdir -p ../../lib
		cp libiolib.dso ../../lib

libiolib.dso:	iolib.o
		$(CC) $(LDFLAGS) -o libiolib.dso iolib.o $(LDLIBS)

iolib.o:	iolib.c iolib.h
		$(CC) $(CFLAGS) -o iolib.o -c iolib.c

clean:
		rm -f *~ iolib.o libiolib.dso
