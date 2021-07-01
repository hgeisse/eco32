#
# Makefile for generating the disk image
#

TOOLS = ../../tools

all:		disk.img.txt randbytes.txt

disk.img.txt:	disk.img
		$(TOOLS)/dump/dump disk.img disk.img.txt

disk.img:	disk.layout randbytes
		$(TOOLS)/mkdisk/mkdisk disk.layout disk.img

randbytes:
		$(TOOLS)/random/random randbytes

randbytes.txt:	randbytes
		$(TOOLS)/dump/dump randbytes randbytes.txt

clean:
		rm -f *~ randbytes disk.img disk.img.txt
		rm -f randbytes.txt
