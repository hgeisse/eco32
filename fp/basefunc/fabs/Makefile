#
# Makefile for implementation and test of a base function
#

all:		fabs

run-few:	fabs
		./fabs -few

run-many:	fabs
		./fabs -many

run-most:	fabs
		./fabs -most

run-all:	fabs
		./fabs -all

fabs:		fabs.c
		gcc -g -Wall -o fabs fabs.c -lm

clean:
		rm -f *~ fabs
