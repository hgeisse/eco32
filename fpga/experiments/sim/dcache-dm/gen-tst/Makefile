#
# Makefile for testpattern generator tests
#

BUILD = ../../../../../build

DIRS = 1a2a3a 1a2a3b 1a2a3c 1a2b3a 1a2b3b 1a2b3c 1a2c3a 1a2c3b 1a2c3c \
       1b2a3a 1b2a3b 1b2a3c 1b2b3a 1b2b3b 1b2b3c 1b2c3a 1b2c3b 1b2c3c

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i democache ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
