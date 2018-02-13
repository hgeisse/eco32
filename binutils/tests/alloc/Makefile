#
# Makefile for storage allocation example
# (this is example 4.2 from the book "Linkers and Loaders")
#

BUILD=../../../build

all:
		$(BUILD)/bin/lcc -A -o main.o -c main.s
		$(BUILD)/bin/lcc -A -o calif.o -c calif.s
		$(BUILD)/bin/lcc -A -o mass.o -c mass.s
		$(BUILD)/bin/lcc -A -o newyork.o -c newyork.s
		$(BUILD)/bin/ld -s example.lnk -o prog \
		  main.o calif.o mass.o newyork.o
		$(BUILD)/bin/dof prog >prog.dump

clean:
		rm -f *~ *.o prog prog.dump
