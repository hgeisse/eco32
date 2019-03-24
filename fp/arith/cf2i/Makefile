#
# Makefile for implementation and test of a base function
#

all:		cf2i

run-few:	cf2i
		./cf2i -few

run-most:	cf2i
		./cf2i -most

run-all:	cf2i
		./cf2i -all

cf2i:		cf2i.c
		gcc -g -Wall -o cf2i cf2i.c -lm

clean:
		rm -f *~ cf2i
