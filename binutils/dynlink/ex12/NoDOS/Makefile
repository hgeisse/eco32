#
# Makefile for building NoDOS
#

DIRS = dump random mkdisk system

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

run:
		$(MAKE) -C system run

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
