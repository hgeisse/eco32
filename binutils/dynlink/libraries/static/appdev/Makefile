#
# Makefile for application development
#

CC = gcc
#CFLAGS = -g -Wall
CFLAGS = -Wall

all:		app

run:		app
		./app

app:		app.o
		$(CC) $(CFLAGS) -L../lib -o app app.o -lmylib

app.o:		app.c
		$(CC) $(CFLAGS) -I../include -o app.o -c app.c

clean:
		rm -f *~ app.o app
