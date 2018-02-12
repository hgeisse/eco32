#
# Makefile for binutils tests
#

BUILD = ../../build

DIRS = abs cycle relocs

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
