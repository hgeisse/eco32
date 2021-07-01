#
# Makefile for building NoDOS
#

DIRS = userland disk kernel

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

run:
		$(MAKE) -C kernel run

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
