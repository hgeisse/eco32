BUILD=../../../../build

all:
		$(BUILD)/bin/lcc -Wall -S -o m1.s m1.c
		$(BUILD)/bin/as -o m1.o m1.s
		$(BUILD)/bin/dof -a m1.o >m1.dump

clean:
		rm -f *~ m1.s m1.o m1.dump
