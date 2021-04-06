#
# Makefile for ECO32 link editor
#

BUILD = ../../../../build

CC = gcc
CFLAGS = -g -Wall -I../include
LDFLAGS = -g
LDLIBS = -lm

SRCS = ld.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
BIN = ld

.PHONY:		all install clean

all:		$(BIN)

install:	all
		mkdir -p $(BUILD)/bin
		cp $(BIN) $(BUILD)/bin

$(BIN):		$(OBJS)
		$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LDLIBS)

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

depend.mak:
		$(CC) -MM -MG $(CFLAGS) $(SRCS) >depend.mak

-include depend.mak

clean:
		rm -f *~ $(OBJS) $(BIN) depend.mak
