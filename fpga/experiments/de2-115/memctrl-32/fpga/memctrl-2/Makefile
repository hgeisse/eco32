#
# Makefile for memory test on Terasic DE2-115 board
#

BUILD = ../../../../../../build

.PHONY:		all install clean

all:		build/output_files/memtest.sof

install:	build/output_files/memtest.sof

clean:
		mv build/memtest.qpf .
		mv build/memtest.qsf .
		if [ -f build/memtest.qws ] ; then \
		  mv build/memtest.qws . ; \
		fi
		if [ -f build/memtest.srf ] ; then \
		  mv build/memtest.srf . ; \
		fi
		if [ -f build/output_files/memtest.sof ] ; then \
		  mv build/output_files/memtest.sof . ; \
		fi
		if [ -f build/output_files/memtest.cdf ] ; then \
		  mv build/output_files/memtest.cdf . ; \
		fi
		rm -rf build/*
		mkdir build/output_files
		mv memtest.qpf build
		mv memtest.qsf build
		if [ -f memtest.qws ] ; then \
		  mv memtest.qws build ; \
		fi
		if [ -f memtest.srf ] ; then \
		  mv memtest.srf build ; \
		fi
		if [ -f memtest.sof ] ; then \
		  mv memtest.sof build/output_files ; \
		fi
		if [ -f memtest.cdf ] ; then \
		  mv memtest.cdf build/output_files ; \
		fi
		rm -f *~
