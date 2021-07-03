#
# Makefile for C library
#

BUILD = ../../../../../build
INCL = ../../include
LIB = ../../lib

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib
AS = $(BUILD)/bin/as
AR = $(BUILD)/bin/ar

MODS = string.o stdlib.o stdio.o dirent.o syscalls.o

all:		libc.a
		mkdir -p $(INCL)
		cp *.h $(INCL)
		mkdir -p $(LIB)
		cp libc.a $(LIB)

libc.a:		$(MODS)
		$(AR) -cv libc.a $(MODS)

syscalls.o:	syscalls.s
		$(AS) -o syscalls.o syscalls.s

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $^

clean:
		rm -f *~ libc.a $(MODS)
