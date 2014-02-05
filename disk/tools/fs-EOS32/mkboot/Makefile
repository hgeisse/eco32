#
# Makefile to build the boot block
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		bootblk

install:	bootblk
		mkdir -p $(BUILD)/run/fs-EOS32
		cp bootblk $(BUILD)/run/fs-EOS32

bootblk:	stage1/br.bin stage2/boot.bin
		cat stage1/br.bin stage2/boot.bin >bootblk

stage1/br.bin:
		$(MAKE) -C stage1

stage2/boot.bin:
		$(MAKE) -C stage2

clean:
		$(MAKE) -C stage1 clean
		$(MAKE) -C stage2 clean
		rm -f *~ bootblk
