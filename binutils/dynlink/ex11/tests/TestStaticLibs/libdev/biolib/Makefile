#
# Makefile for library development
#

BUILD = ../../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AR = $(BUILD)/bin/ar

all:		biolib.h libbiolib.a
		mkdir -p ../../include
		cp biolib.h ../../include
		mkdir -p ../../lib
		cp libbiolib.a ../../lib

libbiolib.a:	biolib.o
		$(AR) -cv libbiolib.a biolib.o

biolib.o:	biolib.c biolib.h
		$(CC) $(CFLAGS) -o biolib.o -c biolib.c

clean:
		rm -f *~ biolib.o libbiolib.a
