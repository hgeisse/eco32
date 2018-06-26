BUILD=../../../../build

all:		m1.dump m1pic.dump m2.dump m2pic.dump

m1.dump:	m1.c
		$(BUILD)/bin/lcc -Wall -S -o m1.s m1.c
		$(BUILD)/bin/as -o m1.o m1.s
		$(BUILD)/bin/dof -a m1.o >m1.dump

m1pic.dump:	m1.c
		$(BUILD)/bin/lcc -Wall -Wo-pic -S -o m1pic.s m1.c
		$(BUILD)/bin/as -o m1pic.o m1pic.s
		$(BUILD)/bin/dof -a m1pic.o >m1pic.dump

m2.dump:	m2.c
		$(BUILD)/bin/lcc -Wall -S -o m2.s m2.c
		$(BUILD)/bin/as -o m2.o m2.s
		$(BUILD)/bin/dof -a m2.o >m2.dump

m2pic.dump:	m2.c
		$(BUILD)/bin/lcc -Wall -Wo-pic -S -o m2pic.s m2.c
		$(BUILD)/bin/as -o m2pic.o m2pic.s
		$(BUILD)/bin/dof -a m2pic.o >m2pic.dump

clean:
		rm -f *~
		rm -f m1.s m1.o m1.dump
		rm -f m1pic.s m1pic.o m1pic.dump
		rm -f m2.s m2.o m2.dump
		rm -f m2pic.s m2pic.o m2pic.dump
