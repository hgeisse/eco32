#
# Makefile for NoDOS userland
#

DIRS = libsrc cmdsrc

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -rf include lib cmd txt
		rm -f *~
