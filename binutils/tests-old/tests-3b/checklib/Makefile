#
# Makefile for checking a static library
#

BUILD=../../../build

LIBDIR=../buildlib

all:		list objs check1 check2

list:
		$(BUILD)/bin/ar -tv $(LIBDIR)/librom.a >lib.lst

objs:
		$(BUILD)/bin/ar -xv $(LIBDIR)/librom.a

check1:
		@echo "\nCheck 1:"
		@if cmp -s lib.lst lib.ref ; then \
		  echo "ok:    library listing equal to reference" ; \
		else \
		  echo "error: library listing differs from reference" ; \
		fi

check2:
		@echo "\nCheck 2:"
		@for i in `ls *.o` ; do \
		  if cmp -s $$i $(LIBDIR)/$$i ; then \
		    echo "ok:    $$i" ; \
		  else \
		    echo "error: $$i" ; \
		  fi \
		done

clean:
		rm -f *~ lib.lst *.o
