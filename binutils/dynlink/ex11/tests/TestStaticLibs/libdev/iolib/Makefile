#
# Makefile for library development
#

BUILD = ../../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib -I../biolib
AR = $(BUILD)/bin/ar

all:		types.h stdarg.h iolib.h libiolib.a
		mkdir -p ../../include
		cp types.h ../../include
		cp stdarg.h ../../include
		cp iolib.h ../../include
		mkdir -p ../../lib
		cp libiolib.a ../../lib

libiolib.a:	iolib.o
		$(AR) -cv libiolib.a iolib.o

iolib.o:	iolib.c iolib.h
		$(CC) $(CFLAGS) -o iolib.o -c iolib.c

clean:
		rm -f *~ iolib.o libiolib.a
