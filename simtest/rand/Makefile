#
# Makefile for generating a 64 KB ROM with random contents
#

BUILD = ../../build

SFILE = romrand.s
OFILE = romrand.o
BINFILE = romrand.bin

.PHONY:		all install clean
.PRECIOUS:	$(SFILE) $(OFILE)

all:		$(BINFILE)

install:	$(BINFILE)
		mkdir -p $(BUILD)/simtest
		cp $(BINFILE) $(BUILD)/simtest

$(BINFILE):	$(OFILE)
		$(BUILD)/bin/ld -s rom.lnk -o tmp $<
		$(BUILD)/bin/load tmp $@
		rm -f tmp

$(OFILE):	$(SFILE)
		$(BUILD)/bin/as -o $@ $<

$(SFILE):	gen
		./gen >$(SFILE)

gen:		gen.c
		gcc -Wall -o gen gen.c

clean:
		rm -f *~ gen $(SFILE) $(OFILE) $(BINFILE)
