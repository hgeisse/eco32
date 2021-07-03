#
# Makefile for generating the disk image
#

TOOLS = ../../tools

all:		disk.img.txt

disk.img.txt:	disk.img
		$(TOOLS)/dump/dump disk.img disk.img.txt

disk.img:	disk.layout
		$(TOOLS)/mkdisk/mkdisk disk.layout disk.img

clean:
		rm -f *~ disk.img disk.img.txt
