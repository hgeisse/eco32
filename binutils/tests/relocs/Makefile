#
# Makefile for as/ld relocation tests
#

BUILD = ../../../build

DIRS = H16 L16 R16 R26 W32 C

.PHONY:		all clean

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
