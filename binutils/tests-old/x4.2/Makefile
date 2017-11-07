#
#
#

BUILD=../../build

all:
		$(BUILD)/bin/lcc -A -o main.o -c main.s
		$(BUILD)/bin/lcc -A -o calif.o -c calif.s
		$(BUILD)/bin/lcc -A -o mass.o -c mass.s
		$(BUILD)/bin/lcc -A -o newyork.o -c newyork.s
		$(BUILD)/bin/ld -s example.lnk -o prog -m prog.map \
		  main.o calif.o mass.o newyork.o

clean:
		rm -f *~ *.o prog prog.map
