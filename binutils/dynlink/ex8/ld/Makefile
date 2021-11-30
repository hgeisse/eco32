#
# Makefile for ECO32 link editor
#

BUILD = ../../../../build

CC = gcc
CFLAGS = -g -Wall
CINCLS = -I../include
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
		$(CC) $(CFLAGS) $(CINCLS) -o $@ -c $<

depend.mak:
		$(CC) -MM -MG $(CFLAGS) $(CINCLS) $(SRCS) >depend.mak

-include depend.mak

clean:
		rm -f *~ $(OBJS) $(BIN) depend.mak
