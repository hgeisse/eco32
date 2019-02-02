#
# Makefile for standard include files
#

BUILD = ../../build

.PHONY:		all install clean

all:

install:
		mkdir -p $(BUILD)/include
		cp *.h $(BUILD)/include
		mkdir -p $(BUILD)/include/sys
		cp sys/*.h $(BUILD)/include/sys

clean:
		rm -f *~ sys/*~
