#
# Makefile for floating-point device test on Terasic DE2-115 board
#

PROJECT = ciftest

all:
		@echo "This Makefile can only be used for 'make clean'!"

clean:
		if [ -f build/$(PROJECT).qpf ] ; then \
		  mv build/$(PROJECT).qpf . ; \
		fi
		if [ -f build/$(PROJECT).qsf ] ; then \
		  mv build/$(PROJECT).qsf . ; \
		fi
		if [ -f build/$(PROJECT).srf ] ; then \
		  mv build/$(PROJECT).srf . ; \
		fi
		if [ -f build/$(PROJECT).svf ] ; then \
		  mv build/$(PROJECT).svf . ; \
		fi
		rm -rf build/*
		if [ -f $(PROJECT).qpf ] ; then \
		  mv $(PROJECT).qpf build ; \
		fi
		if [ -f $(PROJECT).qsf ] ; then \
		  mv $(PROJECT).qsf build ; \
		fi
		if [ -f $(PROJECT).srf ] ; then \
		  mv $(PROJECT).srf build ; \
		fi
		if [ -f $(PROJECT).svf ] ; then \
		  mv $(PROJECT).svf build ; \
		fi
		rm -f *~
