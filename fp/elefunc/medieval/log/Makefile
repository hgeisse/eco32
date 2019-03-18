#
# Makefile for implementation and test of an elementary function
#

all:		log

run-few:	log
		./log -few

run-many:	log
		./log -many

log:		log.c
		gcc -g -Wall -o log log.c -lm

clean:
		rm -f *~ log
