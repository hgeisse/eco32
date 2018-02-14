#
# Makefile for building a static library
#

BUILD=../../../../build

SRCS=debug.c getput.c printf.c qsort.c string.c strtoul.c
OBJS=debug.o getput.o printf.o qsort.o string.o strtoul.o
LIB=librom.a

all:		$(LIB)

$(LIB):		$(OBJS)
		$(BUILD)/bin/ar -cv $(LIB) $(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc -A -o $@ -c $<

clean:
		rm -f *~ $(OBJS) $(LIB)
