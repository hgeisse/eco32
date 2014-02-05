#
# Makefile for building standalone test programs
# (which are to be loaded from disk by a boot-loader)
#

BUILD = ../build

DIRS = hello hello2 memsize onetask twotasks-1 twotasks-2 \
       wrtmbr dskchk mkpart shpart

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
