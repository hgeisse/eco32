BUILD=../../build
SIM=../../../../../build/bin/sim

all:
		@echo "please use one of the following targets:"
		@echo "  make runlo"
		@echo "  make runhi"

prog.o:		prog.s
		$(BUILD)/bin/as -o prog.o prog.s

prog:		prog.o
		$(BUILD)/bin/ld -c 0xC0000000 -o prog prog.o

prog.bin:	prog
		$(BUILD)/bin/load prog prog.bin

runlo:		prog.bin
		$(SIM) -i -l prog.bin

runhi:		prog.bin
		$(SIM) -i -l prog.bin -a 0x3000

clean:
		rm -f *~
		rm -f prog.o prog prog.bin
