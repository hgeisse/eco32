#
# Makefile for makefs utility
#

BUILD = ../../../../build

CC = gcc -m32
CFLAGS = -g -Wall
LDFLAGS = -g
LDLIBS = -lm

SRCS = makefs.c walk.c ffs.c mkfs.c ffs_bswap.c ffs_balloc.c \
       ufs_bmap.c ffs_alloc.c ffs_subr.c ffs_tables.c buf.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
BIN = NetBSD-makefs

.PHONY:		all install clean

all:		$(BIN)

install:	$(BIN)
		mkdir -p $(BUILD)/bin
		cp $(BIN) $(BUILD)/bin

$(BIN):		$(OBJS)
		$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LDLIBS)

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

depend.mak:
		$(CC) -MM -MG $(CFLAGS) $(SRCS) > depend.mak

-include depend.mak

clean:
		rm -f *~ $(OBJS) $(BIN) depend.mak
