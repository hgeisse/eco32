#
# Makefile for implementation and test of an elementary function
#

all:		cos

run-few:	cos
		./cos -few

run-special:	cos
		./cos -special

run-many:	cos
		./cos -many

run-most:	cos
		./cos -most

run-all:	cos
		./cos -all

cos:		cos.c
		gcc -g -Wall -o cos cos.c -lm

clean:
		rm -f *~ cos
