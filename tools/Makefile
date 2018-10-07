#
# Makefile for building the tools
#

BUILD = ../build

DIRS = bin2dat bin2dat2 bin2exo bin2mcs bit2exo bit2mcs \
       chrgen dspmem shdspout shserout vcdchk

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
