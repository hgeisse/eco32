#
# Makefile for implementation and test of a base function
#

all:		ldexp

run-few:	ldexp
		./ldexp -few

run-most:	ldexp
		./ldexp -most

run-all:	ldexp
		./ldexp -all

ldexp:		ldexp.c
		gcc -g -Wall -o ldexp ldexp.c -lm

clean:
		rm -f *~ ldexp
