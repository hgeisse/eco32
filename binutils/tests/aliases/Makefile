#
# Makefile for as aliases test
#

BUILD = ../../../build

all:
	$(BUILD)/bin/as -o prog.o prog.s
	$(BUILD)/bin/ld -s standalone.lnk -o prog prog.o

show: all
	$(BUILD)/bin/dof -a prog

compare: all
	$(BUILD)/bin/dof -a prog | grep ^000000.0 | cut -d" " -f3-18 > output.assembler
	diff -Z output.assembler output.target

clean:
	rm -f *~ prog prog.o output.assembler
