#
# Makefile for implementation and test of a base function
#

all:		floor

run-few:	floor
		./floor -few

run-many:	floor
		./floor -many

run-all:	floor
		./floor -all

floor:		floor.c
		gcc -g -Wall -o floor floor.c -lm

clean:
		rm -f *~ floor
