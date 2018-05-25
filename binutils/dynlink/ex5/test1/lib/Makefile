#
# Makefile for static library
#

BUILD = ../../../../../build

SRCS = f1.c f2.c f3.c f4.c f5.c f6.c f7.c f8.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
LIB = libfuncs.a

all:		$(LIB)

$(LIB):		$(OBJS)
		$(BUILD)/bin/ar -c $(LIB) $(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc -A -c -o $@ $<

clean:
		rm -f *~ $(OBJS) $(LIB)
