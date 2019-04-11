#
# Makefile for implementation and test of an elementary function
#

all:		atan

run-few:	atan
		./atan -few

run-special:	atan
		./atan -special

run-many:	atan
		./atan -many

run-most:	atan
		./atan -most

run-all:	atan
		./atan -all

atan:		atan.c
		gcc -g -Wall -o atan atan.c -lm

clean:
		rm -f *~ atan
