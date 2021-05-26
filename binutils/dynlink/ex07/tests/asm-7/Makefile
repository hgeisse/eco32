BUILD=../../build
SIM=../../../../../build/bin/sim

all:
		@echo "please use one of the following targets:"
		@echo "  make showo"
		@echo "  make showx"
		@echo "  make runlo"
		@echo "  make runhi"

prog.o:		prog.s
		$(BUILD)/bin/as -pic -o prog.o prog.s

showo:		prog.o
		$(BUILD)/bin/dof -a prog.o

mod1.o:		mod1.s
		$(BUILD)/bin/as -pic -o mod1.o mod1.s

mod2.o:		mod2.s
		$(BUILD)/bin/as -pic -o mod2.o mod2.s

prog:		prog.o mod1.o mod2.o
		$(BUILD)/bin/ld -c 0xC0000000 -o prog prog.o mod1.o mod2.o

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
		rm -f prog.o mod1.o mod2.o prog proglo.bin proghi.bin
