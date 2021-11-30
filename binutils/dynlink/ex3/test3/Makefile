#
# Makefile for storage allocation example
# (small segments which need padding)
#

BUILD=../../../../build

all:
		$(BUILD)/bin/lcc -A -o main.o -c main.s
		$(BUILD)/bin/lcc -A -o mass.o -c mass.s
		../ld -d -o prog main.o mass.o
		$(BUILD)/bin/dof -a prog >prog.dump

clean:
		rm -f *~ *.o prog prog.dump
