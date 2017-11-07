#
# Makefile
#

BUILD=../../build

all:
		$(BUILD)/bin/lcc -A -o mod1.o -c mod1.c
		cp mod1.o mod1dbl.o
		$(BUILD)/bin/lcc -A -o mod2.o -c mod2.c
		cp mod2.o mod2dbl.o
		$(BUILD)/bin/ld -o prog -m prog.map mod1.o mod2.o
		#$(BUILD)/bin/ld -o prog -m prog.map mod1.o
		#$(BUILD)/bin/ld -o prog -m prog.map mod2.o
		#$(BUILD)/bin/ld -o prog -m prog.map mod1.o mod1dbl.o
		#$(BUILD)/bin/ld -o prog -m prog.map mod2dbl.o mod2.o

clean:
		rm -f *~ mod1.o mod1dbl.o mod2.o mod2dbl.o prog prog.map
