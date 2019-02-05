#
# Makefile for implementation and test of floating-point arithmetic
#

all:		mul

run:		mul
		./mul

mul:		mul.c
		gcc -g -Wall -o mul mul.c -lm

clean:
		rm -f *~ mul
