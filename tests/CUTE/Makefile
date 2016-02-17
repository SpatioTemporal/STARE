
# With automatic test detection, this file may not be strictly necessary.

.PHONY: CuteTests

include ../../Makefile

ifeq ($(ARCH),LINUX)
	BOOST_FLAGS ?= -I/opt/scidb/15.7/3rdparty/boost_1_54_0/include
	BOOST_LIBS  ?= -L/opt/scidb/15.7/3rdparty/boost_1_54_0/lib
else ifeq ($(ARCH),DARWIN)
	BOOST_FLAGS ?= -I/opt/local/include
	BOOST_LIBS  ?= -L/opt/local/lib
else
	BOOST_FLAGS ?= -I/opt/local/include
	BOOST_LIBS  ?= -L/opt/local/lib
endif

CXX_FLAGS += -I${TOPDIR}/cute ${BOOST_FLAGS}
CLX_FLAGS += ${BOOST_LIBS} -L../../unix -lhstm

CuteTests: test
	./test

test: test.o libs
	$(CXX) -o test test.o $(CLX_FLAGS)
	
test.o: Test.cpp
	$(CXX) $(CXX_FLAGS) -o test.o Test.cpp
	
clean-test:
	-rm -f *.o test
