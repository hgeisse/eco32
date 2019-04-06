#
# Makefile for implementation and test of an elementary function
#

all:		tanh

run-few:	tanh
		./tanh -few

run-special:	tanh
		./tanh -special

run-many:	tanh
		./tanh -many

run-most:	tanh
		./tanh -most

run-all:	tanh
		./tanh -all

tanh:		tanh.c
		gcc -g -Wall -o tanh tanh.c -lm

clean:
		rm -f *~ tanh
