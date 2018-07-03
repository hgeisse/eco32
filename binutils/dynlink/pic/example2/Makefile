BUILD=../../../../build

LCC=$(BUILD)/bin/lcc -target=eco32/linux
AS=$(BUILD)/bin/as
DOF=$(BUILD)/bin/dof

all:		m1.dump m1pic.dump m2.dump m2pic.dump

m1.dump:	m1.c
		$(LCC) -Wall -S -o m1.s m1.c
		$(AS) -o m1.o m1.s
		$(DOF) -a m1.o >m1.dump

m1pic.dump:	m1.c
		$(LCC) -Wall -Wo-pic -S -o m1pic.s m1.c
		$(AS) -o m1pic.o m1pic.s
		$(DOF) -a m1pic.o >m1pic.dump

m2.dump:	m2.c
		$(LCC) -Wall -S -o m2.s m2.c
		$(AS) -o m2.o m2.s
		$(DOF) -a m2.o >m2.dump

m2pic.dump:	m2.c
		$(LCC) -Wall -Wo-pic -S -o m2pic.s m2.c
		$(AS) -o m2pic.o m2pic.s
		$(DOF) -a m2pic.o >m2pic.dump

clean:
		rm -f *~
		rm -f m1.s m1.o m1.dump
		rm -f m1pic.s m1pic.o m1pic.dump
		rm -f m2.s m2.o m2.dump
		rm -f m2pic.s m2pic.o m2pic.dump
