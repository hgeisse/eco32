#
# Makefile for as/ld cyclic reference tests
#

BUILD = ../../../build

all:
	$(BUILD)/bin/as -o src1.o src1.s
	$(BUILD)/bin/as -o src2.o src2.s
	$(BUILD)/bin/as -o src3.o src3.s
	$(BUILD)/bin/ld -s standalone.lnk -o cycle src1.o src2.o src3.o
	$(BUILD)/bin/dof -a cycle >cycle.dump

clean:
	rm -f *~ src1.o src2.o src3.o cycle cycle.dump
