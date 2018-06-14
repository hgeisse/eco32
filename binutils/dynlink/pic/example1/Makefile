BUILD=../../../../build

all:
		@echo "please use one of the following targets:"
		@echo "  make runlo-m1"
		@echo "  make runhi-m1"
		@echo "  make runlo-m2"
		@echo "  make runhi-m2"
		@echo "  make runlo-m3"
		@echo "  make runhi-m3"
		@echo "  make runlo-m4"
		@echo "  make runhi-m4"
		@echo "  make runlo-m5"
		@echo "  make runhi-m5"

build-m1:
		$(BUILD)/bin/as -o m1.o m1.s
		$(BUILD)/bin/ld -c 0xC0000000 -o m1 m1.o
		$(BUILD)/bin/load m1 m1.bin

build-m2:
		$(BUILD)/bin/as -o m2.o m2.s
		$(BUILD)/bin/ld -c 0xC0000000 -o m2 m2.o
		$(BUILD)/bin/load m2 m2.bin

build-m3:
		$(BUILD)/bin/as -o m3.o m3.s
		$(BUILD)/bin/ld -c 0xC0000000 -o m3 m3.o
		$(BUILD)/bin/load m3 m3.bin

build-m4:
		$(BUILD)/bin/as -o m4.o m4.s
		$(BUILD)/bin/ld -c 0xC0000000 -o m4 m4.o
		$(BUILD)/bin/load m4 m4.bin

build-m5:
		$(BUILD)/bin/as -o m5.o m5.s
		$(BUILD)/bin/ld -c 0xC0000000 -o m5 m5.o
		$(BUILD)/bin/load m5 m5.bin

runlo-m1:	build-m1
		$(BUILD)/bin/sim -i -l m1.bin

runhi-m1:	build-m1
		$(BUILD)/bin/sim -i -l m1.bin -a 0x3000

runlo-m2:	build-m2
		$(BUILD)/bin/sim -i -l m2.bin

runhi-m2:	build-m2
		$(BUILD)/bin/sim -i -l m2.bin -a 0x3000

runlo-m3:	build-m3
		$(BUILD)/bin/sim -i -l m3.bin

runhi-m3:	build-m3
		$(BUILD)/bin/sim -i -l m3.bin -a 0x3000

runlo-m4:	build-m4
		$(BUILD)/bin/sim -i -l m4.bin

runhi-m4:	build-m4
		$(BUILD)/bin/sim -i -l m4.bin -a 0x3000

runlo-m5:	build-m5
		$(BUILD)/bin/sim -i -l m5.bin

runhi-m5:	build-m5
		$(BUILD)/bin/sim -i -l m5.bin -a 0x3000

clean:
		rm -f *~
		rm -f m1.o m1 m1.bin
		rm -f m2.o m2 m2.bin
		rm -f m3.o m3 m3.bin
		rm -f m4.o m4 m4.bin
		rm -f m5.o m5 m5.bin
