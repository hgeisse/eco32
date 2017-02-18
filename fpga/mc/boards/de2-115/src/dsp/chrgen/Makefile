#
# Makefile for generating a Verilog init file
# for the display's character generator
#

BUILD = ..

.PHONY:		all install clean

all:		chrgen_rom.init

install:	chrgen_rom.init
		cp chrgen_rom.init $(BUILD)

chrgen_rom.init:	mkinit
			./mkinit

mkinit:		mkinit.c font-8x16
		gcc -g -Wall -o mkinit mkinit.c

clean:
		rm -f *~ mkinit
		rm -f chrgen_rom.init
