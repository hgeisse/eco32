BUILD = ../../../build

all:		task1

task1:		c0.o task1.o putchar.o
		$(BUILD)/bin/ld -o task1 c0.o task1.o putchar.o

c0.o:		c0.s
		$(BUILD)/bin/as -o c0.o c0.s

task1.o:	task1.c
		$(BUILD)/bin/lcc -A -c -o task1.o task1.c

putchar.o:	putchar.s
		$(BUILD)/bin/as -o putchar.o putchar.s

clean:
		rm -f *~ c0.o task1.o putchar.o task1
