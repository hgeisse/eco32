#
# Makefile for static library demo
#

BUILD=../../../build

all:		main

lib/libfuncs.a:
		$(MAKE) -C lib all

main:		lib/libfuncs.a main.o
		$(BUILD)/bin/lcc -A -Wo-ldmap=main.map \
		  -Llib -o main main.o -lfuncs

%.o:		%.c
		$(BUILD)/bin/lcc -A -Ilib -c -o $@ $<

clean:
		$(MAKE) -C lib clean
		rm -f *~ *.o main main.map
