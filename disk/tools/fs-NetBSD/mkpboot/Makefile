#
# Makefile to build the partition boot record
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		pbr.bin

install:	pbr.bin
		mkdir -p $(BUILD)/run/fs-NetBSD
		cp pbr.bin $(BUILD)/run/fs-NetBSD

pbr.bin:	pbr
		$(BUILD)/bin/load -p pbr pbr.bin

pbr:		pbr.o
		$(BUILD)/bin/ld -s stage1.lnk -o pbr pbr.o

pbr.o:		pbr.s
		$(BUILD)/bin/as -o pbr.o pbr.s

clean:
		rm -f *~ pbr.o pbr pbr.bin
