#
# Makefile to build the boot block (with an empty disklabel)
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		bootblk

install:	bootblk
		mkdir -p $(BUILD)/run/fs-NetBSD
		cp bootblk $(BUILD)/run/fs-NetBSD

bootblk:	stage1/br.bin stage2/boot.bin
		dd if=/dev/zero of=./zeroes bs=512 count=1
		cat stage1/br.bin zeroes stage2/boot.bin >bootblk

stage1/br.bin:
		$(MAKE) -C stage1

stage2/boot.bin:
		$(MAKE) -C stage2

clean:
		$(MAKE) -C stage1 clean
		$(MAKE) -C stage2 clean
		rm -f *~ zeroes bootblk
