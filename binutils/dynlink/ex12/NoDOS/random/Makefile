#
# Makefile for generating a file of random bytes
#

all:
		gcc -g -Wall -o random random.c
		./random
		../dump/dump randbytes randbytes.txt

clean:
		rm -f *~ random randbytes randbytes.txt
