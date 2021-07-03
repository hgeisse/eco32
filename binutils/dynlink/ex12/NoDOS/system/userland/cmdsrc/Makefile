#
# Makefile for NoDOS commands
#

DIRS = mini hello argret halt ls cat man chkdsk

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
