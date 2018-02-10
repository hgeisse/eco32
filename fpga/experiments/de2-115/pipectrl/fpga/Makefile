#
# Makefile for pipeline test on Terasic DE2-115 board
#

BUILD = ../../../../../build

.PHONY:		all install clean

all:		build/output_files/pipetest.sof

install:	build/output_files/pipetest.sof

clean:
		mv build/pipetest.qpf .
		mv build/pipetest.qsf .
		if [ -f build/pipetest.qws ] ; then \
		  mv build/pipetest.qws . ; \
		fi
		if [ -f build/pipetest.srf ] ; then \
		  mv build/pipetest.srf . ; \
		fi
		if [ -f build/output_files/pipetest.sof ] ; then \
		  mv build/output_files/pipetest.sof . ; \
		fi
		if [ -f build/output_files/pipetest.cdf ] ; then \
		  mv build/output_files/pipetest.cdf . ; \
		fi
		rm -rf build/*
		mkdir build/output_files
		mv pipetest.qpf build
		mv pipetest.qsf build
		if [ -f pipetest.qws ] ; then \
		  mv pipetest.qws build ; \
		fi
		if [ -f pipetest.srf ] ; then \
		  mv pipetest.srf build ; \
		fi
		if [ -f pipetest.sof ] ; then \
		  mv pipetest.sof build/output_files ; \
		fi
		if [ -f pipetest.cdf ] ; then \
		  mv pipetest.cdf build/output_files ; \
		fi
		rm -f *~
