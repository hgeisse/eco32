#
# Makefile for error tests
#

BUILD=../../../build

all:		noerror

noerror:	mod1.o mod2.o
		$(BUILD)/bin/ld -s me.lnk -o prog -m prog.map mod1.o mod2.o

error1:		mod1.o
		$(BUILD)/bin/ld -s me.lnk -o prog -m prog.map mod1.o

error2:		mod2.o
		$(BUILD)/bin/ld -s me.lnk -o prog -m prog.map mod2.o

error3:		mod1.o mod1dbl.o
		$(BUILD)/bin/ld -s me.lnk -o prog -m prog.map mod1.o mod1dbl.o

error4:		mod2dbl.o mod2.o
		$(BUILD)/bin/ld -s me.lnk -o prog -m prog.map mod2dbl.o mod2.o

mod1.o:		mod1.c
		$(BUILD)/bin/lcc -A -o mod1.o -c mod1.c

mod1dbl.o:	mod1.o
		cp mod1.o mod1dbl.o

mod2.o:		mod2.c
		$(BUILD)/bin/lcc -A -o mod2.o -c mod2.c

mod2dbl.o:	mod2.o
		cp mod2.o mod2dbl.o

clean:
		rm -f *~ mod1.o mod1dbl.o mod2.o mod2dbl.o prog prog.map
