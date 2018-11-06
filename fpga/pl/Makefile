#
# Makefile for pipeline implementations of ECO32
#

BUILD = ../../build

.PHONY:		all install clean

all:
		$(MAKE) -C boards all

install:
		$(MAKE) -C boards install

clean:
		$(MAKE) -C boards clean
		rm -f *~
		rm -f src/*~
		rm -f src/cpu/*~
		rm -f src/cpu/if/*~
		rm -f src/cpu/of/*~
		rm -f src/cpu/ex/*~
		rm -f src/cpu/ma/*~
		rm -f src/cpu/wb/*~
