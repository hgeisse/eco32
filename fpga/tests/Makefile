#
# Makefile for tests
#

BUILD = ../../build

DIRS = test_000 test_001 test_002 test_003 \
       test_004 test_005 test_006 test_007 \
       test_010 test_011 \
       test_050 test_051 test_052 test_053 test_054 \
       test_090 \
       test_100 test_101

all:
		for i in  $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:
		for i in  $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done

clean:
		for i in  $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
