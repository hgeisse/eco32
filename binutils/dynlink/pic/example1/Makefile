BUILD=../../../../build

all:
		@echo "please use one of the following targets:"
		@echo "  make runlo-m1"
		@echo "  make runhi-m1"
		@echo "  make runlo-m2"
		@echo "  make runhi-m2"

build-m1:
		$(BUILD)/bin/as -o m1.o m1.s
		../../ex6/ld -c 0xC0000000 -o m1 m1.o
		$(BUILD)/bin/load m1 m1.bin

build-m2:
		$(BUILD)/bin/as -o m2.o m2.s
		../../ex6/ld -c 0xC0000000 -o m2 m2.o
		$(BUILD)/bin/load m2 m2.bin

runlo-m1:	build-m1
		$(BUILD)/bin/sim -i -l m1.bin

runhi-m1:	build-m1
		$(BUILD)/bin/sim -i -l m1.bin -a 0x3000

runlo-m2:	build-m2
		$(BUILD)/bin/sim -i -l m2.bin

runhi-m2:	build-m2
		$(BUILD)/bin/sim -i -l m2.bin -a 0x3000

clean:
		rm -f *~
		rm -f m1.o m1 m1.bin
		rm -f m2.o m2 m2.bin
