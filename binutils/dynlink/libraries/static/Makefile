#
# Makefile for creating and using a static library
#

all:
		$(MAKE) -C libdev all
		$(MAKE) -C appdev all

run:
		$(MAKE) -C appdev run

clean:
		$(MAKE) -C appdev clean
		$(MAKE) -C libdev clean
		rm -rf include
		rm -rf lib
		rm -f *~
