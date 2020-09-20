#
# Makefile for testing the I2C master controller
#

SRCS = i2c_tst.v i2c_mc.v

all:		show

show:		i2c_tst
		./i2c_tst
		gtkwave dump.vcd i2c_tst.cfg

i2c_tst:	$(SRCS)
		iverilog -Wall -o i2c_tst $(SRCS)

clean:
		rm -f *~ i2c_tst dump.vcd
