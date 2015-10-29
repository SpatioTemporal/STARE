
# With automatic test detection, this file may not be strictly necessary.

.PHONY: CuteTests

include ../../Makefile

CFLAGS += -I$(TOPDIR)/cute -I/opt/local/include

LDFLAGS += -L/opt/local/lib

CuteTests: test
	./test

test: test.o
	$(CXX) -o test $(LDFLAGS) test.o
	
test.o: Test.cpp
	$(CXX) $(CFLAGS) -o test.o Test.cpp
	
clean-test:
	-rm -f *.o test
