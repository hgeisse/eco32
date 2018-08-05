BUILD=../../../../build

all:
		@echo "please use one of the following targets:"
		@echo "  make runlo"
		@echo "  make runhi"
		@echo "  make runhi-reloc"

prog.o:		prog.s
		$(BUILD)/bin/as -o prog.o prog.s

prog:		prog.o
		$(BUILD)/bin/ld -c 0xC0000000 -o prog prog.o

prog.bin:	prog
		$(BUILD)/bin/load prog prog.bin

runlo:		prog.bin
		$(BUILD)/bin/sim -i -l prog.bin

runhi:		prog.bin
		$(BUILD)/bin/sim -i -l prog.bin -a 0x3000

runhi-reloc:	prog.bin
		cp prog.bin prog-reloc.bin
		dd if=patchbyte of=prog-reloc.bin bs=1 seek=4110 conv=notrunc
		$(BUILD)/bin/sim -i -l prog-reloc.bin -a 0x3000

clean:
		rm -f *~
		rm -f prog.o prog prog.bin prog-reloc.bin
