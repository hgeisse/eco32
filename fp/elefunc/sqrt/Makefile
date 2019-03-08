#
# Makefile for implementation and test of an elementary function
#

all:		sqrt

run-few:	sqrt
		./sqrt -few

run-many:	sqrt
		./sqrt -many

sqrt:		sqrt.c
		gcc -g -Wall -o sqrt sqrt.c -lm

clean:
		rm -f *~ sqrt
