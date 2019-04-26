# With automatic test detection, this file may not be strictly necessary.

.PHONY: CuteTests

include ../../Makefile

ifeq "$(origin CUTEDIR)" "undefined"
$(error "CUTEDIR NOT SET, EXITING.")
endif

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

CXX_FLAGS += -I${CUTEDIR}/cute ${BOOST_FLAGS} -I.
CLX_FLAGS += ${BOOST_LIBS} -L../../unix -lSTARE

CuteTests: test
	./test

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

test: $(OBJS)
	$(CXX) -o test $(OBJS) $(CLX_FLAGS)

#test.o: Test.cpp
#	$(CXX) $(CXX_FLAGS) -o test.o Test.cpp

%.o : %.cpp
	$(CXX) -c $(CXX_FLAGS) $< -o $@

clean-test:
	-rm -f *.o test

local_version:
	make --version
	
local_echo:
	@echo OBJS: ${OBJS}	
	@echo CXX_FLAGS: $(CXX_FLAGS)
	@echo CLX_FLAGS: $(CLX_FLAGS)

#	echo CXX_FLAGS2: ${CXX_FLAGS}
