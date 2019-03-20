#
# Makefile for implementation and test of a base function
#

all:		ceil

run-few:	ceil
		./ceil -few

run-most:	ceil
		./ceil -most

run-all:	ceil
		./ceil -all

ceil:		ceil.c
		gcc -g -Wall -o ceil ceil.c -lm

clean:
		rm -f *~ ceil
