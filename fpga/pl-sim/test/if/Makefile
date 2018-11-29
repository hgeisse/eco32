#
# Makefile for testing the IF stage of the pipeline
#

all:		test

test:		test.c vaddr.dat page.dat frame.dat rom.dat
		gcc -Wall -o test test.c

vaddr.dat:
		cp -p ../../src/cpu/if/vaddr.dat .

page.dat:
		cp -p ../../src/cpu/if/page.dat .

frame.dat:
		cp -p ../../src/cpu/if/frame.dat .

rom.dat:
		cp -p ../../rom.dat .

clean:
		rm -f *~ test vaddr.dat page.dat frame.dat rom.dat
