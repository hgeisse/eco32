#
# Makefile for building library tests
#

BUILD = ../build

DIRS = startup libc libm

.PHONY:		all install clean

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done
		mkdir -p $(BUILD)/libtest
		cp Makefile.run $(BUILD)/libtest/Makefile

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
