
set (
  STARE_SrcFiles
  ${STARE_SrcFiles}
  BitShiftNameEncoding.C
  EmbeddedLevelNameEncoding.C
  General.cpp
  HtmRange.cpp
  HtmRangeIterator.cpp
  HtmRangeMultiLevel.cpp
  HtmRangeMultiLevelIterator.cpp
  Htmio.cpp
  HstmRange.C
  NameEncoding.C
  RangeConvex.cpp
  SkipList.cpp
  SkipListElement.cpp
  SpatialConstraint.cpp
  SpatialDomain.cpp
  SpatialEdge.cpp
  SpatialException.cpp
  SpatialGeneral.C
  SpatialIndex.cpp
  SpatialInterface.cpp
  SpatiallyAdaptiveDataCover.C
  SpatialVector.cpp
  SpatialRotation.C
  STARE.C
  SpatialRange.cpp
  TemporalIndex.cpp
  TemporalWordFormat1.cpp
  VarStr.C
  ../external/erfa-single-file/erfa.c )
  
# TODO Trouble with VarStr.hpp
# Var.Str.C is a kluge because I don't know how to get CMake to handle the hpp files correctly.
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects

add_library( STARE STATIC ${STARE_SrcFiles} )

#
# Moved up so we can make python tests depend on building the python interface.
# jhrg 3/20/20
#
#
# Moved up so we can make python tests depend on building the python interface.
# jhrg 3/20/20
#
# SWIG integration of PySTARE

# Adding the two 'POLICY' clauses protects older versions of CMake
# from policies they don;t know about. jhrg 8/27/19
if (SWIG_FOUND AND POLICY CMP0078 AND POLICY CMP0086)
	# UseSWIG_TARGET_NAME_PREFERENCE
	#cmake_minimum_required(VERSION 3.13 FATAL ERROR)
	cmake_policy( SET CMP0078 NEW )
	cmake_policy( SET CMP0086 NEW )
	
        # message("e-pid: " "$ENV{PYTHON_INCLUDE_DIRS}")
	if( DEFINED ENV{PYTHON_INCLUDE_DIRS} )
	  string( REPLACE ":" ";" INCLUDE_LIST $ENV{PYTHON_INCLUDE_DIRS} )
	  include_directories( ${INCLUDE_LIST} )
	else()
	  message("PYTHON_INCLUDE_DIRS not found. Guessing where to find python includes.")
	  include_directories( "/usr/include/python3.7m" "/usr/include/python3.6m" "/usr/lib64/python3.7/site-packages/numpy/core/include"  )
  	endif()

    add_library(PySTAREA SHARED PySTARE.cpp)
    # set_target_properties( PySTARE PROPERTIES PREFIX "" )
    #? set target properties(PySTARE PROPERTIES LIBRARY OUTPUT DIRECTORY ${PROJECT BINARY DIR}/..) 
    # target_link_libraries( PySTAREA "/usr/lib64/liberfa.so" )
    
    # SWIG
    find_package(SWIG REQUIRED)
    include(${SWIG_USE_FILE})
    find_package(PythonLibs)
    include_directories(${PYTHON_INCLUDE_DIRS})
    
    # set_property(SOURCE mymod.i PROPERTY CPLUSPLUS ON)
	# swig_add_library(mymod LANGUAGE python SOURCES mymod.i)
	
	set_property(SOURCE PySTARE.i PROPERTY CPLUSPLUS ON)
	# swig_add_library(mymod LANGUAGE python SOURCES mymod.i)

    # swig_add_library( PySTARE TYPE SHARED LANGUAGE python SOURCES PySTARE.i )
    swig_add_library( pystare TYPE SHARED LANGUAGE python SOURCES PySTARE.i )
    swig_link_libraries( pystare PySTAREA )
    swig_link_libraries( pystare ${PYTHON_LIBRARIES} )
    swig_link_libraries( pystare STARE )
endif()

install(TARGETS STARE DESTINATION lib)
