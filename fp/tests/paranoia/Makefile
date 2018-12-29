#
# Makefile
#

all:		spparanoia dpparanoia

spparanoia:	paranoia.c
		gcc -g -Wall -o spparanoia -DSingle paranoia.c -lm

dpparanoia:	paranoia.c
		gcc -g -Wall -o dpparanoia paranoia.c -lm

clean:
		rm -f *~ spparanoia dpparanoia
