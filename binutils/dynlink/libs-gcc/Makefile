#
# Makefile for creating and using a library
#

all:
		$(MAKE) -C static all
		$(MAKE) -C dynamic all

run-static:
		$(MAKE) -C static run

run-dynamic:
		$(MAKE) -C dynamic run

clean:
		$(MAKE) -C static clean
		$(MAKE) -C dynamic clean
		rm -f *~
