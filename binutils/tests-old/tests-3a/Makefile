#
# Makefile for tests
#

DIRS=test_00 test_01

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
