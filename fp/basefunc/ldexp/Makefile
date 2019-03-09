#
# Makefile for implementation and test of a base function
#

all:		ldexp

run-few:	ldexp
		./ldexp -few

run-many:	ldexp
		./ldexp -many

run-most:	ldexp
		./ldexp -most

ldexp:		ldexp.c
		gcc -g -Wall -o ldexp ldexp.c -lm

clean:
		rm -f *~ ldexp
