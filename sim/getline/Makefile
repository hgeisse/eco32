#
# Makefile for the getline library
#

CC      = gcc -m32
CFLAGS  = -O -DPOSIX
LDFLAGS = -L.
LDLIBS  = -lgetline

.PHONY:		all install clean

all:		libgetline.a testgl

install:	libgetline.a testgl

testgl:		libgetline.a testgl.o
		$(CC) $(LDFLAGS) $(CFLAGS) -o testgl testgl.o $(LDLIBS)

libgetline.a:	getline.o
		ar cr libgetline.a getline.o

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

clean:
		rm -f *~ *.o *.a testgl
