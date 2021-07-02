#
# Makefile for NoDOS userland libraries
#

DIRS = crt libc

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -rf include lib cmd
		rm -f *~
