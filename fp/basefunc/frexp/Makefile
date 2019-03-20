#
# Makefile for implementation and test of a base function
#

all:		frexp

run-few:	frexp
		./frexp -few

run-most:	frexp
		./frexp -most

run-all:	frexp
		./frexp -all

frexp:		frexp.c
		gcc -g -Wall -o frexp frexp.c -lm

clean:
		rm -f *~ frexp
