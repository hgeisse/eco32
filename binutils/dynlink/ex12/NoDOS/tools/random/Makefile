#
# Makefile for generating a file of random bytes
#

all:		randbytes.txt

randbytes.txt:	randbytes
		../dump/dump randbytes randbytes.txt

randbytes:	random
		./random randbytes

random:		random.c
		gcc -g -Wall -o random random.c

clean:
		rm -f *~ random randbytes randbytes.txt
