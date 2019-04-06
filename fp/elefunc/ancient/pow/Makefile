#
# Makefile for implementation and test of an elementary function
#

all:		pow

run-few:	pow
		./pow -few

run-special:	pow
		./pow -special

run-many:	pow
		./pow -many

run-most:	pow
		./pow -most

run-all:	pow
		./pow -all

pow:		pow.c
		gcc -g -Wall -o pow pow.c -lm

clean:
		rm -f *~ pow
