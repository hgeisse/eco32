#
# Makefile for ECO32 ROM monitor
#

BUILD = ../../build

SRC = start.s main.c command.c instr.c asm.c disasm.c \
      load.c boot.c cpu.c mmu.c getline.c romlib.c \
      keyboard.s ../kbdtbls/kbdtbls.s display.s \
      serial.s sctio-ctl.s sctio-ser.s end.s

.PHONY:		all install clean

all:		monitor.bin monitor.mcs

install:	monitor.bin monitor.mcs
		mkdir -p $(BUILD)/monitor
		cp monitor.bin $(BUILD)/monitor
		cp monitor.mcs $(BUILD)/monitor

monitor.mcs:	monitor.bin
		$(BUILD)/bin/bin2mcs 0x00000000 monitor.bin monitor.mcs

monitor.bin:	copy/copy.bin $(SRC)
		$(BUILD)/bin/lcc -A -Wo-rom -Wl-rc -Wl0xC0000000 \
		  -Wl-rd -Wl0xC000C000 \
		  -Wl-m -Wlmonitor.map \
		  -o prelim.bin $(SRC)
		cat copy/copy.bin prelim.bin >monitor.bin
		rm prelim.bin

copy/copy.bin:
		$(MAKE) -C copy all

clean:
		rm -f *~
		rm -f monitor.map monitor.bin
		rm -f monitor.mcs
		$(MAKE) -C copy clean
