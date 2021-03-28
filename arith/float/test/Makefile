#
# Makefile for test environment
#

DIR = TestFloat-3e/build/Linux-x86_64-GCC

all:		SoftFloat-3e TestFloat-3e
		make -C SoftFloat-3e/build/Linux-x86_64-GCC
		make -C TestFloat-3e/build/Linux-x86_64-GCC
		mkdir -p bin
		cp $(DIR)/testfloat bin
		cp $(DIR)/testfloat_gen bin
		cp $(DIR)/testfloat_ver bin
		cp $(DIR)/testsoftfloat bin
		cp $(DIR)/timesoftfloat bin

SoftFloat-3e:
		unzip ./orig/SoftFloat-3e.zip

TestFloat-3e:
		unzip ./orig/TestFloat-3e.zip

clean:
		rm -f *~
		rm -rf SoftFloat-3e
		rm -rf TestFloat-3e
		rm -rf bin
