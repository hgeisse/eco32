BUILD=../../../../build

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
		$(BUILD)/bin/lcc -o prog.o -c prog.s

showo:		prog.o
		$(BUILD)/bin/dof -a prog.o

prog:		start.o prog.o
		$(BUILD)/bin/ld -c 0xC0000000 -o prog start.o prog.o

showx:		prog
		$(BUILD)/bin/dof -a prog

prog.bin:	prog
		$(BUILD)/bin/load prog prog.bin

runlo:		prog.bin
		$(BUILD)/bin/sim -i -l prog.bin

runhi:		prog.bin
		$(BUILD)/bin/sim -i -l prog.bin -a 0x3000

clean:
		rm -f *~
		rm -f start.o prog.s prog.o prog prog.bin
