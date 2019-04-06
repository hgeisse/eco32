#
# Makefile for implementation and test of an elementary function
#

all:		exp

run-few:	exp
		./exp -few

run-special:	exp
		./exp -special

run-many:	exp
		./exp -many

run-most:	exp
		./exp -most

run-all:	exp
		./exp -all

exp:		exp.c
		gcc -g -Wall -o exp exp.c -lm

clean:
		rm -f *~ exp
