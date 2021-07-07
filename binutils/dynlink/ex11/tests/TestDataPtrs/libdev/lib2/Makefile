#
# Makefile for library development
#

BUILD = ../../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-pic
LDFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-shared

all:		lib2.h liblib2.dso
		mkdir -p ../../include
		cp lib2.h ../../include
		mkdir -p ../../lib
		cp liblib2.dso ../../lib

liblib2.dso:	lib2.o
		$(CC) $(LDFLAGS) -o liblib2.dso lib2.o

lib2.o:		lib2.c lib2.h
		$(CC) $(CFLAGS) -o lib2.o -c lib2.c

clean:
		rm -f *~ lib2.o liblib2.dso
