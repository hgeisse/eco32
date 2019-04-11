#
# Makefile for implementation and test of an elementary function
#

all:		acos

run-few:	acos
		./acos -few

run-special:	acos
		./acos -special

run-many:	acos
		./acos -many

run-most:	acos
		./acos -most

run-all:	acos
		./acos -all

acos:		acos.c
		gcc -g -Wall -o acos acos.c -lm

clean:
		rm -f *~ acos
