
# With automatic test detection, this file may not be strictly necessary.

.PHONY: CuteTests

include ../../Makefile

BOOST_FLAGS = -I/opt/scidb/15.7/3rdparty/boost_1_54_0/include
CXX_FLAGS += -I${TOPDIR}/cute -I/opt/local/include ${BOOST_FLAGS}

BOOST_LIBS = -L/opt/scidb/15.7/3rdparty/boost_1_54_0/lib
CLX_FLAGS += -L${BOOST_LIBS} -L/opt/local/lib -L../../unix -lhstm

CuteTests: test
	./test

test: test.o libs
	$(CXX) -o test test.o $(CLX_FLAGS)
	
test.o: Test.cpp
	$(CXX) $(CXX_FLAGS) -o test.o Test.cpp
	
clean-test:
	-rm -f *.o test
