#
# Makefile for creating and using a static library
#

all:
		$(MAKE) -C libdev all
		$(MAKE) -C appdev all

runlo:		all
		$(MAKE) -C appdev runlo

runhi:		all
		$(MAKE) -C appdev runhi

clean:
		$(MAKE) -C appdev clean
		$(MAKE) -C libdev clean
		rm -rf include
		rm -rf lib
		rm -f *~
