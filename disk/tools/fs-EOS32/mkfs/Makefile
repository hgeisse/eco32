#
# Makefile for mkfs utility
#

BUILD = ../../../../build

CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -g
LDLIBS = -lm

SRCS = mkfs.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
BIN = EOS32-mkfs

.PHONY:		all install clean

all:		$(BIN)

install:	$(BIN)
		mkdir -p $(BUILD)/bin
		cp $(BIN) $(BUILD)/bin
		mkdir -p $(BUILD)/run/fs-EOS32
		cp root.fsys $(BUILD)/run/fs-EOS32
		cp usr.fsys $(BUILD)/run/fs-EOS32

$(BIN):		$(OBJS)
		$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LDLIBS)

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

depend.mak:
		$(CC) -MM -MG $(CFLAGS) $(SRCS) >depend.mak

-include depend.mak

clean:
		rm -f *~ $(OBJS) $(BIN) depend.mak
