#
# Makefile for implementation and test of an elementary function
#

all:		tan

run-few:	tan
		./tan -few

run-many:	tan
		./tan -many

run-most:	tan
		./tan -most

run-all:	tan
		./tan -all

tan:		tan.c
		gcc -g -Wall -o tan tan.c -lm

clean:
		rm -f *~ tan
