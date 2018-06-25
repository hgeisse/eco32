BUILD=../../../../build

all:		m1.dump m1pic.dump

m1.dump:	m1.c
		$(BUILD)/bin/lcc -Wall -S -o m1.s m1.c
		$(BUILD)/bin/as -o m1.o m1.s
		$(BUILD)/bin/dof -a m1.o >m1.dump

m1pic.dump:	m1.c
		$(BUILD)/bin/lcc -Wall -S -o m1pic.s m1.c
		$(BUILD)/bin/as -o m1pic.o m1pic.s
		$(BUILD)/bin/dof -a m1pic.o >m1pic.dump

clean:
		rm -f *~
		rm -f m1.s m1.o m1.dump
		rm -f m1pic.s m1pic.o m1pic.dump
