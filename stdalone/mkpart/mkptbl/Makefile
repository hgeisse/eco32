#
# Makefile for generating a partition table
#

all:		parttbl

parttbl:	mkptbl disk.part
		./mkptbl disk.part parttbl

mkptbl:		mkptbl.c
		gcc -m32 -g -Wall -o mkptbl mkptbl.c

clean:
		rm -f *~ mkptbl parttbl
