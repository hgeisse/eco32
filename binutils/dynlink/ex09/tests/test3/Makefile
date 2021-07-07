#
# Makefile for creating and using a dynamic library
#

all:
		$(MAKE) -C libdev all
		$(MAKE) -C appdev all

clean:
		$(MAKE) -C appdev clean
		$(MAKE) -C libdev clean
		rm -rf include
		rm -rf lib
		rm -f *~
