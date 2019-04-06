#
# Makefile for implementation and test of an elementary function
#

all:		log

run-few:	log
		./log -few

run-special:	log
		./log -special

run-many:	log
		./log -many

run-most:	log
		./log -most

run-all:	log
		./log -all

log:		log.c
		gcc -g -Wall -o log log.c -lm

clean:
		rm -f *~ log
