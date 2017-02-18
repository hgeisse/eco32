#
# Makefile for ECO32 on Terasic DE2-115 board
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		build/output_files/eco32.sof

install:	build/output_files/eco32.sof
		#$(BUILD)/bin/bit2exo 0x180000 \
		#  build/eco32.bit build/eco32.exo
		#mkdir -p $(BUILD)/eco32/xsa-xst-3
		#cp build/eco32.bit $(BUILD)/eco32/xsa-xst-3
		#cp build/eco32.exo $(BUILD)/eco32/xsa-xst-3
		#cp doc/README $(BUILD)/eco32/xsa-xst-3

clean:
		mv build/eco32.qpf .
		mv build/eco32.qsf .
		mv build/eco32.qws .
		mv build/eco32.srf .
		#mv build/output_files .
		rm -rf build/*
		mv eco32.qpf build
		mv eco32.qsf build
		mv eco32.qws build
		mv eco32.srf build
		#mv output_files build
		rm -f *~
