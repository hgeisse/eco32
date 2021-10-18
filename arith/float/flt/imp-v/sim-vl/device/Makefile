#
# Makefile for testing floating-point arithmetic
#

SRCS = main.cpp fpflt/fpflt.v

all:		fpserver

fpserver:	$(SRCS)
		verilator -cc -exe -build -o fpserver $(SRCS)
		cp obj_dir/fpserver .

clean:
		rm -f *~
		rm -rf obj_dir
		rm -f fpserver
