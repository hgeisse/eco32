#
# Makefile for C library
#

BUILD = ../../../../../build
INCL = ../../include
LIB = ../../lib

AR = $(BUILD)/bin/ar
AS = $(BUILD)/bin/as

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
		$(BUILD)/bin/lcc -o $@ -c $^

clean:
		rm -f *~ libc.a $(MODS)
