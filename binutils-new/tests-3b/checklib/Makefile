#
# Makefile for checking a static library
#

BUILD=../../../build

LIBDIR=../buildlib

all:		objs check

check:
		@for i in `ls *.o` ; do \
		  echo -n $$i ; \
		  if cmp -s $$i $(LIBDIR)/$$i ; then \
		    echo "\t\tok" ; \
		  else \
		    echo "\t\tcompare error" ; \
		  fi \
		done

objs:
		$(BUILD)/bin/ar -xv $(LIBDIR)/librom.a

clean:
		rm -f *~ *.o
