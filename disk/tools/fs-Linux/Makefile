#
# Makefile for Linux disk tools
#

BUILD = ../../../build

DIRS = mkboot mvstrt

.PHONY:		all install clean

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done
		mkdir -p $(BUILD)/run/fs-Linux
		cp Makefile.run $(BUILD)/run/fs-Linux/Makefile

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
