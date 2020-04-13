#
# Makefile for building standalone test programs
# (see README for different ways to run them)
#

BUILD = ../build

DIRS = hello hello2 bottles memsize memtest onetask twotasks-1 \
       twotasks-2 dskchk dskchk2 wrtmbr dmpmbr mkpart shpart \
       dactest dhrystone sdctest atomics coremark kbdtest \
       mousetest gfxchk1 gfxchk2

.PHONY:		all install clean

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
