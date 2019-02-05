#
# Makefile for basic floating-point tests
#

# number of test cases
NUM = 10000000
# test level (1 or 2)
LVL = 2

all:		hauser basic
		@echo "Please use one of the following invocations:"
		@echo "  make run-simple"
		@echo "  make run-add-spec"
		@echo "  make run-add"
		@echo "  make run-sub-spec"
		@echo "  make run-sub"
		@echo "  make run-mul-spec"
		@echo "  make run-mul"
		@echo "  make run-div-spec"
		@echo "  make run-div"
		@echo "  make run-sqrt-spec"
		@echo "  make run-sqrt"

run-simple:	basic
		./basic

run-add-spec:	hauser basic
		cat spec2.dat | \
		./basic -add | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_add

run-add:	hauser basic
		TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./basic -add | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_add

run-sub-spec:	hauser basic
		cat spec2.dat | \
		./basic -sub | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_sub

run-sub:	hauser basic
		TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./basic -sub | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_sub

run-mul-spec:	hauser basic
		cat spec2.dat | \
		./basic -mul | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_mul

run-mul:	hauser basic
		TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./basic -mul | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_mul

run-div-spec:	hauser basic
		cat spec2.dat | \
		./basic -div | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_div

run-div:	hauser basic
		TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./basic -div | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_div

run-sqrt-spec:	hauser basic
		cat spec1.dat | \
		./basic -sqrt | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_sqrt

run-sqrt:	hauser basic
		TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 1 | \
		./basic -sqrt | \
		TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_sqrt

hauser:		SoftFloat-3e TestFloat-3e

SoftFloat-3e:
		unzip ../tests/hauser/SoftFloat-3e.zip
		$(MAKE) -C SoftFloat-3e/build/Linux-386-GCC

TestFloat-3e:
		unzip ../tests/hauser/TestFloat-3e.zip
		$(MAKE) -C TestFloat-3e/build/Linux-386-GCC

basic:		basic.c
		gcc -g -Wall -o basic basic.c -lm

clean:
		rm -rf SoftFloat-3e
		rm -rf TestFloat-3e
		rm -f *~ basic
