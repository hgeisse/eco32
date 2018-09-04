BUILD=../../../../build

all:
		@echo "please use one of the following targets:"
		@echo "  make showo"
		@echo "  make showx"
		@echo "  make runlo"
		@echo "  make runhi"

prog.o:		prog.s
		$(BUILD)/bin/as -p -o prog.o prog.s

showo:		prog.o
		$(BUILD)/bin/dof -a prog.o

prog:		prog.o
		$(BUILD)/bin/ld -c 0xC0000000 -o prog prog.o

showx:		prog
		$(BUILD)/bin/dof -a prog

proglo.bin:	prog
		$(BUILD)/bin/load -v prog proglo.bin

runlo:		proglo.bin
		$(BUILD)/bin/sim -i -l proglo.bin

proghi.bin:	prog
		$(BUILD)/bin/load -v -l 0x3000 prog proghi.bin

runhi:		proghi.bin
		$(BUILD)/bin/sim -i -l proghi.bin -a 0x3000

clean:
		rm -f *~
		rm -f prog.o prog proglo.bin proghi.bin
