#
# Makefile for implementation and test of an elementary function
#

all:		cosh

run-few:	cosh
		./cosh -few

run-many:	cosh
		./cosh -many

run-most:	cosh
		./cosh -most

run-all:	cosh
		./cosh -all

cosh:		cosh.c
		gcc -g -Wall -o cosh cosh.c -lm

clean:
		rm -f *~ cosh
