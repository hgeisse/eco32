#
# Makefile for implementation and test of an elementary function
#

all:		sqrt

run-few:	sqrt
		./sqrt -few

run-special:	sqrt
		./sqrt -special

run-many:	sqrt
		./sqrt -many

run-most:	sqrt
		./sqrt -most

run-all:	sqrt
		./sqrt -all

sqrt:		sqrt.c
		gcc -g -Wall -o sqrt sqrt.c -lm

clean:
		rm -f *~ sqrt
