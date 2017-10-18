#
# Makefile for ECO32 linking loader
#

BUILD = ../../build

CC = gcc
CFLAGS = -g -Wall -Iscript
LDFLAGS = -g -L$(LIBDIR)
LDLIBS = -lreadscript -lm

SRCS = ld.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
BIN = ld

LIBDIR = script
LIB = libreadscript.a

.PHONY:		all install clean

all:		$(LIBDIR)/$(LIB) $(BIN)

$(LIBDIR)/$(LIB):
		$(MAKE) -C $(LIBDIR) all

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
		$(MAKE) -C $(LIBDIR) clean
		rm -f *~ $(OBJS) $(BIN) depend.mak
