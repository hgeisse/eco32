#
# Makefile for storage allocation example
# (more than one segment in segment group)
#

BUILD=../../../../../build

all:		patch
		$(BUILD)/bin/lcc -A -o main.o -c main.s
		$(BUILD)/bin/lcc -A -o calif.o -c calif.s
		cp calif.o filac.o
		./patch filac.o 0xab4 0x65
		./patch filac.o 0xab5 0x64
		./patch filac.o 0xab6 0x6F
		./patch filac.o 0xab7 0x63
		./patch filac.o 0xaba 0x61
		./patch filac.o 0xabb 0x74
		./patch filac.o 0xabc 0x61
		./patch filac.o 0xabd 0x64
		./patch filac.o 0xac0 0x73
		./patch filac.o 0xac1 0x73
		./patch filac.o 0xac2 0x62
		$(BUILD)/bin/lcc -A -o mass.o -c mass.s
		cp mass.o ssam.o
		./patch ssam.o 0x982 0x65
		./patch ssam.o 0x983 0x64
		./patch ssam.o 0x984 0x6F
		./patch ssam.o 0x985 0x63
		./patch ssam.o 0x988 0x61
		./patch ssam.o 0x989 0x74
		./patch ssam.o 0x98A 0x61
		./patch ssam.o 0x98B 0x64
		./patch ssam.o 0x98E 0x73
		./patch ssam.o 0x98F 0x73
		./patch ssam.o 0x990 0x62
		$(BUILD)/bin/lcc -A -o newyork.o -c newyork.s
		../../ld/ld -o prog \
		  main.o filac.o calif.o mass.o ssam.o newyork.o
		$(BUILD)/bin/dof -a prog >prog.dump

patch:		patch.c
		gcc -g -Wall -o patch patch.c

clean:
		rm -f *~ *.o prog prog.dump patch
