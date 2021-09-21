#
# Makefile for test environment
#

TEST_DIR = TestFloat-3e/build/Linux-x86_64-GCC
SOFT_DIR = SoftFloat-3e/build/Linux-x86_64-GCC
INCL_DIR = SoftFloat-3e/source/include

all:		SoftFloat-3e TestFloat-3e HardFloat-1
		make -C SoftFloat-3e/build/Linux-x86_64-GCC
		make -C TestFloat-3e/build/Linux-x86_64-GCC
		mkdir -p bin
		cp $(TEST_DIR)/testfloat bin
		cp $(TEST_DIR)/testfloat_gen bin
		cp $(TEST_DIR)/testfloat_ver bin
		cp $(TEST_DIR)/testsoftfloat bin
		cp $(TEST_DIR)/timesoftfloat bin
		mkdir -p include
		cp $(INCL_DIR)/softfloat.h include
		cp $(INCL_DIR)/softfloat_types.h include
		mkdir -p lib
		cp $(TEST_DIR)/testfloat.a lib
		cp $(SOFT_DIR)/softfloat.a lib

SoftFloat-3e:
		unzip ./orig/SoftFloat-3e.zip

TestFloat-3e:
		unzip ./orig/TestFloat-3e.zip

HardFloat-1:
		unzip ./orig/HardFloat-1.zip

clean:
		rm -f *~
		rm -rf SoftFloat-3e
		rm -rf TestFloat-3e
		rm -rf HardFloat-1
		rm -rf bin
		rm -rf include
		rm -rf lib
