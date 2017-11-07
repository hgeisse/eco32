#
# Makefile
#

BUILD=../../build

all:		main

lib/libfuncs.a:
		$(MAKE) -C lib all

main:		lib/libfuncs.a main.o
		#$(BUILD)/bin/lcc -A -Llib -o main main.o -lfuncs
		$(BUILD)/bin/ld -o main main.o lib/libfuncs.a

%.o:		%.c
		$(BUILD)/bin/lcc -A -Ilib -c -o $@ $<

clean:
		$(MAKE) -C lib clean
		rm -f *~ *.o main
