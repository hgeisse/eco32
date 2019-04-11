#
# Makefile for implementation and test of an elementary function
#

all:		asin

run-few:	asin
		./asin -few

run-special:	asin
		./asin -special

run-many:	asin
		./asin -many

run-most:	asin
		./asin -most

run-all:	asin
		./asin -all

asin:		asin.c
		gcc -g -Wall -o asin asin.c -lm

clean:
		rm -f *~ asin
