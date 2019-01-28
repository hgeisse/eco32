#
# Makefile for C standard library
#

BUILD = ../../../build

SRCS = stdio.c
OBJS = $(patsubst %.c,%.o,$(SRCS))

.PHONY:		all install clean

all:		$(OBJS)

install:	$(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc -A -I../../include -o $@ -c $<

clean:
		rm -f *~ $(OBJS)
