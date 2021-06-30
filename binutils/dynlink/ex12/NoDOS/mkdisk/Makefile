#
# Makefile for building a super simple disk image
#

all:		mkdisk
		./mkdisk disk.layout disk.img
		../dump/dump disk.img disk.img.txt

mkdisk:		mkdisk.c
		gcc -g -Wall -o mkdisk mkdisk.c

clean:
		rm -f *~ mkdisk disk.img disk.img.txt
