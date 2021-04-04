#
# Makefile for object file I/O
#

BUILD=../../../../../build

all:
		$(BUILD)/bin/lcc -A -o test1.o -c test1.s
		$(BUILD)/bin/dof -a test1.o >test1.dump
		../../ld/ld -o test2.o test1.o
		$(BUILD)/bin/dof -a test2.o >test2.dump
		../../ld/ld -o test3.o test2.o
		$(BUILD)/bin/dof -a test3.o >test3.dump

clean:
		rm -f *~ test1.o test1.dump
		rm -f test2.o test2.dump test3.o test3.dump
