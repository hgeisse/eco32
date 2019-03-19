#
# Makefile for implementation and test of a base function
#

all:		ci2f

run-few:	ci2f
		./ci2f -few

run-most:	ci2f
		./ci2f -most

run-all:	ci2f
		./ci2f -all

ci2f:		ci2f.c
		gcc -g -Wall -o ci2f ci2f.c -lm

clean:
		rm -f *~ ci2f
