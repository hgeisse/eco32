#
# Makefile for EOS32 disk tools
#

BUILD = ../../../build

DIRS = mkboot mkfs shfs

.PHONY:		all install clean

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done
		mkdir -p $(BUILD)/run/fs-EOS32
		cp Makefile.run $(BUILD)/run/fs-EOS32/Makefile

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
