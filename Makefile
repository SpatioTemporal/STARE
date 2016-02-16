
.PHONY: libs version

TOPDIR  ?= /Users/mrilee/git/hstm
CXX     = g++
CC      = gcc
INCDIR  = $(TOPDIR)/include
OBJ     = $(TOPDIR)/unix
SRC     = $(TOPDIR)/trcomp.cpp
SRCS    = ${TOPDIR}/src
SHELL   = /bin/csh
APP_SRCS= ${TOPDIR}/app
APPS    = range2list fillout toid toname par xptest filter lookup

CFLAGS_FROM_CONFIGURE_SCRIPT  = -g -Wall -I${INCDIR} -D_BOOL_EXISTS -D__unix -UDIAGNOSE

#CFLAGS  = -c -g -Wall -I${INCDIR} -D_BOOL_EXISTS -D__macosx -UDIAGNOSE
#CFLAGS_BASE = -O3 -Wall -I${INCDIR} -D_BOOL_EXISTS -D__macosx -UDIAGNOSE
CFLAGS_BASE = ${CFLAGS_FROM_CONFIGURE_SCRIPT}
CFLAGS  = -c ${CFLAGS_BASE}
CXX_FLAGS  = -std=c++11 ${CFLAGS}
CLX_FLAGS  = -std=c++11
#
# Should not have -O3 with debug
# -DHAVE_BITLIST

SKIPOB = \
   ${OBJ}/SkipListElement.o \
   ${OBJ}/SkipList.o \
   ${OBJ}/HtmRange.o \
   ${OBJ}/SpatialException.o \
   ${OBJ}/NameEncoding.o \
   ${OBJ}/BitShiftNameEncoding.o \
   ${OBJ}/EmbeddedLevelNameEncoding.o \
   ${TOPDIR}/cc_aux.o 

OBJECTS = \
   ${OBJ}/SpatialGeneral.o \
   ${OBJ}/NameEncoding.o \
   ${OBJ}/BitShiftNameEncoding.o \
   ${OBJ}/EmbeddedLevelNameEncoding.o \
   ${OBJ}/SpatialIndex.o \
   ${OBJ}/SpatialConstraint.o \
   ${OBJ}/sqlInterface.o \
   ${OBJ}/SpatialVector.o \
   ${OBJ}/SpatialInterface.o \
   ${OBJ}/SpatialEdge.o \
   ${OBJ}/SpatialException.o \
   ${OBJ}/SpatialDomain.o \
   ${OBJ}/RangeConvex.o \
   ${OBJ}/Htmio.o \
   ${OBJ}/SkipListElement.o \
   ${OBJ}/SkipList.o \
   ${OBJ}/HtmRange.o \
   ${OBJ}/HtmRangeIterator.o \
   ${TOPDIR}/cc_aux.o 

#  ${OBJ}/SpatialConvex.o 
#   ${OBJ}/BitList.o

all: intersect filter libs CuteTestsTop
# example

CuteTestsTop: libs
	$(MAKE) -C tests/CUTE clean-test
	$(MAKE) -C tests/CUTE CuteTests

example: ${OBJ}/example.o ${OBJECTS}
	$(CXX) -o $@  ${OBJ}/example.o ${OBJECTS}

sqlExample: ${OBJ}/sqlExample.o ${OBJECTS}
	$(CXX) -o $@  ${OBJ}/sqlExample.o ${OBJECTS}

intersect: ${OBJ}/intersect.o ${OBJECTS}
	$(CXX) -g -o $@  ${OBJ}/intersect.o ${OBJECTS} 
	
	
libs: ${OBJ}/libhstm.a

${OBJ}/libhstm.a: ${OBJECTS}
	ar -r $@ ${OBJECTS}
	ar -s $@
#---------------------------------------------
${OBJ}/htmlookup.o: ${APP_SRCS}/htmlookup.c
	$(CC) $(CFLAGS) ${APP_SRCS}/htmlookup.c -o $@
htmlookup: ${OBJ}/htmlookup.o ${TOPDIR}/cc_aux.o
	$(CC) -g -o $@  ${OBJ}/htmlookup.o ${TOPDIR}/cc_aux.o
#---------------------------------------------
${OBJ}/toname.o: ${APP_SRCS}/toname.c
	$(CC) $(CFLAGS) ${APP_SRCS}/toname.c -o $@
toname: ${OBJ}/toname.o ${TOPDIR}/cc_aux.o
	$(CC) -g -o $@  ${OBJ}/toname.o ${TOPDIR}/cc_aux.o
#---------------------------------------------
${OBJ}/toid.o: ${APP_SRCS}/toid.c
	$(CC) $(CFLAGS) ${APP_SRCS}/toid.c -o $@
toid: ${OBJ}/toid.o ${TOPDIR}/cc_aux.o
	$(CC) -g -o $@  ${OBJ}/toid.o ${TOPDIR}/cc_aux.o
#---------------------------------------------
${OBJ}/fillout.o: ${APP_SRCS}/fillout.c
	$(CC) $(CFLAGS) ${APP_SRCS}/fillout.c -o $@
fillout: ${OBJ}/fillout.o ${TOPDIR}/cc_aux.o
	$(CC) -g -o $@  ${OBJ}/fillout.o ${TOPDIR}/cc_aux.o
#---------------------------------------------
${OBJ}/range2list.o: ${APP_SRCS}/range2list.c
	$(CC) $(CFLAGS) ${APP_SRCS}/range2list.c -o $@
range2list: ${OBJ}/range2list.o ${TOPDIR}/cc_aux.o
	$(CC) -g -o $@  ${OBJ}/range2list.o ${TOPDIR}/cc_aux.o
#---------------------------------------------
lookup: ${OBJ}/lookup.o ${OBJECTS}
	$(CXX) -g -o $@  ${OBJ}/lookup.o ${OBJECTS} 

filter: filter.o $(SKIPOB)
	$(CXX) $(CLX_FLAGS) -o $@ filter.o $(SKIPOB)

filter.o: filter.cpp
	$(CXX) $(CXX_FLAGS) filter.cpp -o $@
	
testrun:
	make -f Makefile intersect; rm -f tmp.out
	./intersect  20 testInputIntersect > tmp.out
	diff -q tmp.out correctOutputIntersect
	./intersect -varlength 20 testInputIntersect > tmp.out 
	diff -q tmp.out correctVarout
	./intersect -varlength 20 testTiny > tmp.out
	diff -q tmp.out correctTinyVarout
	./intersect  20 testTiny > tmp.out
	diff -q tmp.out correctTinyOut
testcompress:
	make -f Makefile intersect filter
	./intersect 20 testInputIntersect | filter 1 
	./intersect -nranges 1 20 testInputIntersect
vis:
	echo "C 80 20 30" > /tmp/h1
	./intersect -varlength 20 testInputIntersect >> /tmp/h1

xptest: ${OBJ}/xptest.o ${OBJECTS}
	$(CXX) -g -o $@  ${OBJ}/xptest.o ${OBJECTS}

${OBJ}/lookup.o: ${APP_SRCS}/lookup.cpp
	$(CXX) $(CXX_FLAGS) ${APP_SRCS}/lookup.cpp -o $@

test1: ${OBJ}/test1.o ${OBJECTS}
	$(CXX) -g -o $@  ${OBJ}/test1.o ${OBJECTS}

${OBJ}/xptest.o: ${APP_SRCS}/xptest.cpp
	$(CXX) $(CXX_FLAGS) ${APP_SRCS}/xptest.cpp -o $@

${OBJ}/intersect.o: ${APP_SRCS}/intersect.cpp
	$(CXX) $(CXX_FLAGS) ${APP_SRCS}/intersect.cpp -o $@

${OBJ}/test1.o: ${APP_SRCS}/test1.cpp
	$(CXX) $(CXX_FLAGS) ${APP_SRCS}/test1.cpp -o $@

${OBJ}/example.o: ${APP_SRCS}/example.cpp
	$(CXX) $(CXX_FLAGS) ${APP_SRCS}/example.cpp -o $@

${OBJ}/sqlExample.o: ${APP_SRCS}/sqlExample.cpp
	$(CXX) $(CXX_FLAGS) ${APP_SRCS}/sqlExample.cpp -o $@

${OBJ}/SpatialGeneral.o: ${SRCS}/SpatialGeneral.C
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialGeneral.C -o $@

${OBJ}/SpatialIndex.o: ${SRCS}/SpatialIndex.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialIndex.cpp -o $@

${OBJ}/SpatialConstraint.o: ${SRCS}/SpatialConstraint.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialConstraint.cpp -o $@

${OBJ}/sqlInterface.o: ${SRCS}/sqlInterface.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/sqlInterface.cpp -o $@

${OBJ}/SpatialVector.o: ${SRCS}/SpatialVector.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialVector.cpp -o $@

${OBJ}/SpatialInterface.o: ${SRCS}/SpatialInterface.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialInterface.cpp -o $@

${OBJ}/SpatialEdge.o: ${SRCS}/SpatialEdge.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialEdge.cpp -o $@

${OBJ}/SpatialException.o: ${SRCS}/SpatialException.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialException.cpp -o $@

${OBJ}/SpatialDomain.o: ${SRCS}/SpatialDomain.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialDomain.cpp -o $@

${OBJ}/SpatialConvex.o: ${SRCS}/SpatialConvex.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SpatialConvex.cpp -o $@

${OBJ}/RangeConvex.o: ${SRCS}/RangeConvex.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/RangeConvex.cpp -o $@

${OBJ}/Htmio.o: ${SRCS}/Htmio.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/Htmio.cpp -o $@

${OBJ}/SkipListElement.o: ${SRCS}/SkipListElement.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SkipListElement.cpp -o $@

${OBJ}/SkipList.o: ${SRCS}/SkipList.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/SkipList.cpp -o $@

${OBJ}/HtmRange.o: ${SRCS}/HtmRange.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/HtmRange.cpp -o $@

${OBJ}/HtmRangeIterator.o: ${SRCS}/HtmRangeIterator.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/HtmRangeIterator.cpp -o $@

${OBJ}/NameEncoding.o: ${SRCS}/NameEncoding.C
	$(CXX) $(CXX_FLAGS) $< -o $@

${OBJ}/BitShiftNameEncoding.o: ${SRCS}/BitShiftNameEncoding.C
	$(CXX) $(CXX_FLAGS) $< -o $@

${OBJ}/EmbeddedLevelNameEncoding.o: ${SRCS}/EmbeddedLevelNameEncoding.C
	$(CXX) $(CXX_FLAGS) $< -o $@

${OBJ}/XXX.o: ${SRCS}/XXX.cpp
	$(CXX) $(CXX_FLAGS) ${SRCS}/XXX.cpp -o $@

${TOPDIR}/cc_aux.o: ${TOPDIR}/cc_aux.c
	$(CC) -c $(CFLAGS) ${TOPDIR}/cc_aux.c -o $@

cc_intersect.o: ${TOPDIR}/cc_intersect.c
	$(CC) -c cc_intersect.c

clean:
	rm -f $(OBJECTS) ${OBJ}/hello.o  $(OBJ)/intersect.o $(OBJ)/test1.o $(OBJ)/fin.o ${OBJ}/NameEncoding.o ${OBJ}/BitShiftNameEncoding.o ${OBJ}/libhstm.a

cleanAll:
	-rm -f $(OBJECTS) ${OBJ}/*.o
	-rm -f a.out intersect sqlExample trcomp 
	-rm -f $(APPS)
	-rm -f tmp.out *.o *~ 
	-rm -f ${APP_SRCS}/*~
#
tarball:
	source mktarball.csh
par: HtmParse.o
	$(CXX) -g -o $@ HtmParse.o  ${OBJECTS}

HtmParse.o: HtmParse.cpp HtmParse.h
	$(CXX) $(CXX_FLAGS) -I. HtmParse.cpp
%.o: %.cpp
	$(CXX) -c $(CXX_FLAGS) $< -o $@
	
version:
	$(CXX) --version

