#
# Makefile for library development
#

CC = gcc
#CFLAGS = -g -Wall
CFLAGS = -Wall
AR = ar

all:		mylib.h libmylib.a
		mkdir -p ../include
		cp mylib.h ../include
		mkdir -p ../lib
		cp libmylib.a ../lib

libmylib.a:	mylib.o
		$(AR) crs libmylib.a mylib.o

mylib.o:	mylib.c mylib.h
		$(CC) $(CFLAGS) -o mylib.o -c mylib.c

clean:
		rm -f *~ mylib.o libmylib.a
