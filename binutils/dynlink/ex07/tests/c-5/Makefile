BUILD=../../build
SIM=../../../../../build/bin/sim

all:
		@echo "please use one of the following targets:"
		@echo "  make shows"
		@echo "  make showo"
		@echo "  make showx"
		@echo "  make runlo"
		@echo "  make runhi"

start.o:	start.s
		$(BUILD)/bin/as -o start.o start.s

prog.s:		prog.c
		$(BUILD)/bin/lcc -pic -o prog.s -S prog.c

shows:		prog.s
		cat prog.s

prog.o:		prog.s
		$(BUILD)/bin/lcc -pic -o prog.o -c prog.s

showo:		prog.o
		$(BUILD)/bin/dof -a prog.o

mod1.s:		mod1.c
		$(BUILD)/bin/lcc -pic -o mod1.s -S mod1.c

mod1.o:		mod1.s
		$(BUILD)/bin/lcc -pic -o mod1.o -c mod1.s

prog:		start.o prog.o mod1.o
		$(BUILD)/bin/ld -c 0xC0000000 -m prog.map \
		  -o prog start.o prog.o mod1.o

showx:		prog
		$(BUILD)/bin/dof -a prog

proglo.bin:	prog
		$(BUILD)/bin/load -v prog proglo.bin

runlo:		proglo.bin
		$(SIM) -i -l proglo.bin

proghi.bin:	prog
		$(BUILD)/bin/load -v -l 0x3000 prog proghi.bin

runhi:		proghi.bin
		$(SIM) -i -l proghi.bin -a 0x3000

clean:
		rm -f *~
		rm -f start.o prog.s prog.o mod1.s mod1.o
		rm -f prog.map prog proglo.bin proghi.bin
