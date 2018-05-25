#
# Makefile for static library demo
#

BUILD=../../../../build

all:		main

lib/libfuncs.a:
		$(MAKE) -C lib all

main:		lib/libfuncs.a main.o
		../ld -s main -o main -m main.map main.o lib/libfuncs.a

%.o:		%.c
		$(BUILD)/bin/lcc -A -c -o $@ $<

clean:
		$(MAKE) -C lib clean
		rm -f *~ *.o main main.map
