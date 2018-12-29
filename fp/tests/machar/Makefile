#
# Makefile
#

all:		spmachar dpmachar

spmachar:	machar.c
		gcc -g -Wall -o spmachar -DSP machar.c

dpmachar:	machar.c
		gcc -g -Wall -o dpmachar -DDP machar.c

clean:
		rm -f *~ spmachar dpmachar
