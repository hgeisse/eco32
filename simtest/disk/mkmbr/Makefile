#
# Makefile for assembling the master boot record
#

BUILD = ../../../build

SFILES = $(wildcard *.s)
OBJFILES = $(patsubst %.s,%.o,$(SFILES))
BINFILES = $(patsubst %.s,%.bin,$(SFILES))

.PHONY:		all install clean
.PRECIOUS:	$(OBJFILES)

all:		$(BINFILES)

install:	$(BINFILES)
		mkdir -p $(BUILD)/simtest
		cp $(BINFILES) $(BUILD)/simtest

%.bin:		%.o
		$(BUILD)/bin/ld -s ram.lnk -o tmp $<
		$(BUILD)/bin/load tmp $@
		rm -f tmp

%.o:		%.s
		$(BUILD)/bin/as -o $@ $<

clean:
		rm -f *~ *.bin *.o
