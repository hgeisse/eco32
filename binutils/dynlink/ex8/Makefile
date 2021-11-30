#
# Makefile for building the assembler tool chain
#

BUILD = ../../../build

DIRS = as dof ld load
#DIRS = ar as dof ld load

.PHONY:		all install clean

all:		compiler
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:	compiler
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done

compiler:
		mkdir -p $(BUILD)/bin
		$(MAKE) -C lcc BUILDDIR=`pwd`/$(BUILD)/bin \
		  HOSTFILE=etc/eco32-linux.c lcc
		$(MAKE) -C lcc BUILDDIR=`pwd`/$(BUILD)/bin all
		rm -f $(BUILD)/bin/*.c
		rm -f $(BUILD)/bin/*.o
		rm -f $(BUILD)/bin/*.a

clean:
		rm -f lcc/lburg/gram.c
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
