#
# Makefile for implementation and test of an elementary function
#

all:		sinh

run-few:	sinh
		./sinh -few

run-special:	sinh
		./sinh -special

run-many:	sinh
		./sinh -many

run-most:	sinh
		./sinh -most

run-all:	sinh
		./sinh -all

sinh:		sinh.c
		gcc -g -Wall -o sinh sinh.c -lm

clean:
		rm -f *~ sinh
