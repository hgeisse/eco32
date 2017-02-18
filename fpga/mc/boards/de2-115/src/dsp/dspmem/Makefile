#
# Makefile for generating a Verilog init file
# for the display's screen memory
#

BUILD = ..

.PHONY:		all install clean

all:		dspmem_att.init dspmem_chr.init

install:	dspmem_att.init dspmem_chr.init
		cp dspmem_att.init $(BUILD)
		cp dspmem_chr.init $(BUILD)

dspmem_att.init:	mkinit
			./mkinit

dspmem_chr.init:	mkinit
			./mkinit

mkinit:		mkinit.c screen
		gcc -g -Wall -o mkinit mkinit.c

clean:
		rm -f *~ mkinit
		rm -f dspmem_att.init dspmem_chr.init
