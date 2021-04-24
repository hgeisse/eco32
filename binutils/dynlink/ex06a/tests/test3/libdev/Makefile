#
# Makefile for library development
#

BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A
AR = $(BUILD)/bin/ar

all:		mylib.h libmylib.a
		mkdir -p ../include
		cp mylib.h ../include
		mkdir -p ../lib
		cp libmylib.a ../lib

libmylib.a:	mylib.o
		$(AR) -cv libmylib.a mylib.o

mylib.o:	mylib.c mylib.h
		$(CC) $(CFLAGS) -o mylib.o -c mylib.c

clean:
		rm -f *~ mylib.o libmylib.a
