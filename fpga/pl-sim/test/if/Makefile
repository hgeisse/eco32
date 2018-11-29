#
# Makefile for testing the IF stage of the pipeline
#

all:		test-if

test-if:	test-if.c vaddr.dat page.dat frame.dat rom.dat
		gcc -Wall -o test-if test-if.c

vaddr.dat:
		cp -p ../../src/cpu/if/vaddr.dat .

page.dat:
		cp -p ../../src/cpu/if/page.dat .

frame.dat:
		cp -p ../../src/cpu/if/frame.dat .

rom.dat:
		cp -p ../../rom.dat .

clean:
		rm -f *~ test-if vaddr.dat page.dat frame.dat rom.dat
