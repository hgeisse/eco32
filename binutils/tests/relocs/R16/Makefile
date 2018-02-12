#
# Makefile for as/ld relocation test
#

BUILD = ../../../../build

all:
	$(BUILD)/bin/as -o test0.o test0.s
	$(BUILD)/bin/as -o test1.o test1.s
	$(BUILD)/bin/as -o test2.o test2.s
	$(BUILD)/bin/ld -s standalone.lnk -o test -m test.map \
	  test0.o test1.o test2.o
	$(BUILD)/bin/dof -a test1.o >test1.dump
	$(BUILD)/bin/dof -a test >test.dump

clean:
	rm -f *~ test0.o test1.o test2.o test test.map
	rm -f test1.dump test.dump
