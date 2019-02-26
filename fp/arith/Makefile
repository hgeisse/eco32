#
# Makefile for implementation and test of floating-point arithmetic
#

BUILD = ../../build

DIRS = h2x rep sqrt

all:		hauser
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:	hauser
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done

hauser:		SoftFloat-3e TestFloat-3e

SoftFloat-3e:
		unzip ../tests/hauser/SoftFloat-3e.zip
		$(MAKE) -C SoftFloat-3e/build/Linux-386-GCC

TestFloat-3e:
		unzip ../tests/hauser/TestFloat-3e.zip
		$(MAKE) -C TestFloat-3e/build/Linux-386-GCC

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -rf SoftFloat-3e
		rm -rf TestFloat-3e
		rm -f *~
