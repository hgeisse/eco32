#
# Makefile for library development
#

BUILD = ../../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-pic -I../lib2
LDFLAGS = -A -Wo-nostdinc -Wo-nostdlib -Wo-shared
LDLIBS = ../lib2/liblib2.dso

all:		lib1.h liblib1.dso
		mkdir -p ../../include
		cp lib1.h ../../include
		mkdir -p ../../lib
		cp liblib1.dso ../../lib

liblib1.dso:	lib1.o
		$(CC) $(LDFLAGS) -o liblib1.dso lib1.o $(LDLIBS)

lib1.o:		lib1.c lib1.h
		$(CC) $(CFLAGS) -o lib1.o -c lib1.c

clean:
		rm -f *~ lib1.o liblib1.dso
