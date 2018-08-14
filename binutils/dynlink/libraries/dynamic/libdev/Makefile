#
# Makefile for library development
#

CC = gcc
#CFLAGS = -g -Wall
CFLAGS = -Wall

all:		mylib.h libmylib.so
		mkdir -p ../include
		cp mylib.h ../include
		mkdir -p ../lib
		cp libmylib.so ../lib

libmylib.so:	mylib.o
		$(CC) -shared -o libmylib.so mylib.o

mylib.o:	mylib.c mylib.h
		$(CC) $(CFLAGS) -o mylib.o -c -fPIC mylib.c

clean:
		rm -f *~ mylib.o libmylib.so
