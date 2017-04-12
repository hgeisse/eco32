#
# Makefile for ECO32 on Terasic DE2-115 board
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		build/output_files/eco32.sof

install:	build/output_files/eco32.sof
		mkdir -p $(BUILD)/eco32/de2-115
		cp build/output_files/eco32.sof $(BUILD)/eco32/de2-115
		cp doc/README $(BUILD)/eco32/de2-115

clean:
		mv build/eco32.qpf .
		mv build/eco32.qsf .
		if [ -f build/eco32.qws ] ; then \
		  mv build/eco32.qws . ; \
		fi
		if [ -f build/eco32.srf ] ; then \
		  mv build/eco32.srf . ; \
		fi
		if [ -f build/output_files/eco32.sof ] ; then \
		  mv build/output_files/eco32.sof . ; \
		fi
		if [ -f build/output_files/eco32.cdf ] ; then \
		  mv build/output_files/eco32.cdf . ; \
		fi
		rm -rf build/*
		mkdir build/output_files
		mv eco32.qpf build
		mv eco32.qsf build
		if [ -f eco32.qws ] ; then \
		  mv eco32.qws build ; \
		fi
		if [ -f eco32.srf ] ; then \
		  mv eco32.srf build ; \
		fi
		if [ -f eco32.sof ] ; then \
		  mv eco32.sof build/output_files ; \
		fi
		if [ -f eco32.cdf ] ; then \
		  mv eco32.cdf build/output_files ; \
		fi
		rm -f *~
