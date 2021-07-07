#
# Makefile for library development
#

BUILD = ../../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-pic
LDFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-shared

all:		biolib.h libbiolib.dso
		mkdir -p ../../include
		cp biolib.h ../../include
		mkdir -p ../../lib
		cp libbiolib.dso ../../lib

libbiolib.dso:	biolib.o
		$(CC) $(LDFLAGS) -o libbiolib.dso biolib.o

biolib.o:	biolib.c biolib.h
		$(CC) $(CFLAGS) -o biolib.o -c biolib.c

clean:
		rm -f *~ biolib.o libbiolib.dso
