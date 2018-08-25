#
# Makefile for C relocation test
#

BUILD = ../../../../build

all:
		@echo "Please use on of these targets:"
		@echo "    no_relocs"
		@echo "    all_relocs"
		@echo "    _code_code_H16L16"
		@echo "    _code_code_R16"
		@echo "    _code_code_R26"
		@echo "    _code_data_H16L16"
		@echo "    _code_bss_H16L16"
		@echo "    _code_symbol_H16L16_1"
		@echo "    _code_symbol_H16L16_2"
		@echo "    _code_symbol_R26"
		@echo "    _data_code_W32"
		@echo "    _data_data_W32"
		@echo "    _data_bss_W32"
		@echo "    _data_symbol_W32_1"
		@echo "    _data_symbol_W32_2"

no_relocs:
		$(BUILD)/bin/lcc -o test.s -S test.c
		$(BUILD)/bin/lcc -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

all_relocs:
		$(BUILD)/bin/lcc -Dall_relocs -o test.s -S test.c
		$(BUILD)/bin/lcc -Dall_relocs -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_code_code_H16L16:
		$(BUILD)/bin/lcc -D_code_code_H16L16 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_code_code_H16L16 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_code_code_R16:
		$(BUILD)/bin/lcc -D_code_code_R16 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_code_code_R16 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_code_code_R26:
		$(BUILD)/bin/lcc -D_code_code_R26 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_code_code_R26 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_code_data_H16L16:
		$(BUILD)/bin/lcc -D_code_data_H16L16 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_code_data_H16L16 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_code_bss_H16L16:
		$(BUILD)/bin/lcc -D_code_bss_H16L16 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_code_bss_H16L16 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_code_symbol_H16L16_1:
		$(BUILD)/bin/lcc -D_code_symbol_H16L16_1 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_code_symbol_H16L16_1 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_code_symbol_H16L16_2:
		$(BUILD)/bin/lcc -D_code_symbol_H16L16_2 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_code_symbol_H16L16_2 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_code_symbol_R26:
		$(BUILD)/bin/lcc -D_code_symbol_R26 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_code_symbol_R26 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_data_code_W32:
		$(BUILD)/bin/lcc -D_data_code_W32 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_data_code_W32 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_data_data_W32:
		$(BUILD)/bin/lcc -D_data_data_W32 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_data_data_W32 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_data_bss_W32:
		$(BUILD)/bin/lcc -D_data_bss_W32 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_data_bss_W32 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_data_symbol_W32_1:
		$(BUILD)/bin/lcc -D_data_symbol_W32_1 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_data_symbol_W32_1 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

_data_symbol_W32_2:
		$(BUILD)/bin/lcc -D_data_symbol_W32_2 -o test.s -S test.c
		$(BUILD)/bin/lcc -D_data_symbol_W32_2 -o test.o -c test.c
		$(BUILD)/bin/dof -a test.o >test.dump

clean:
		rm -f *~ test.s test.o test.dump
