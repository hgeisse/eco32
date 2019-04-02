#
# Makefile for implementation and test of an elementary function
#

all:		sin

run-few:	sin
		./sin -few

run-many:	sin
		./sin -many

run-most:	sin
		./sin -most

run-all:	sin
		./sin -all

sin:		sin.c
		gcc -g -Wall -o sin sin.c -lm

clean:
		rm -f *~ sin
