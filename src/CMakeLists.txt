
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
  TemporalIndex.cpp
  TemporalWordFormat1.cpp
  VarStr.C
  ../external/erfa-single-file/erfa.c )
  
# TODO Trouble with VarStr.hpp
# Var.Str.C is a kluge because I don't know how to get CMake to handle the hpp files correctly.
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects

add_library( STARE STATIC ${STARE_SrcFiles} )

<<<<<<< HEAD
# NB: If this code needs a certain version of SWIG, add that number to
# the 'find_package()' call as a second argument. jhrg 8/27/19
=======

>>>>>>> 363a0572b3372633ee508217531fb12b69299c3d
find_package(SWIG)

# SWIG integration of PySTARE1
<<<<<<< HEAD

# Adding the two 'POLICY' clauses protects older versions of CMake
# from policies they don;t know about. jhrg 8/27/19
if (SWIG_FOUND AND POLICY CMP0078 AND POLICY CMP0086)
	# UseSWIG_TARGET_NAME_PREFERENCE	
=======
if(SWIG_FOUND)
	# UseSWIG_TARGET_NAME_PREFERENCE

	#cmake_minimum_required(VERSION 3.13 FATAL ERROR)
>>>>>>> 363a0572b3372633ee508217531fb12b69299c3d
	cmake_policy( SET CMP0078 NEW )
	cmake_policy( SET CMP0086 NEW )
	
        # message("e-pid: " "$ENV{PYTHON_INCLUDE_DIRS}")
	if( DEFINED ENV{PYTHON_INCLUDE_DIRS} )
	  string( REPLACE ":" ";" INCLUDE_LIST $ENV{PYTHON_INCLUDE_DIRS} )
	  include_directories( ${INCLUDE_LIST} )
	else()
	  message("PYTHON_INCLUDE_DIRS not found. Guessing where to find python includes.")
	  include_directories( "/usr/include/python3.7m" "/usr/lib64/python3.7/site-packages/numpy/core/include"  )
  	endif()

    add_library(PySTARE1A SHARED PySTARE1.cpp)
   
    # set_target_properties( PySTARE1 PROPERTIES PREFIX "" )
    #? set target properties(PySTARE1 PROPERTIES LIBRARY OUTPUT DIRECTORY ${PROJECT BINARY DIR}/..) 
    # target_link_libraries( PySTARE1A "/usr/lib64/liberfa.so" )

    
    # SWIG
    find_package(SWIG REQUIRED)
    include(${SWIG_USE_FILE})
    find_package(PythonLibs)
    include_directories(${PYTHON_INCLUDE_DIRS})
    
    # set_property(SOURCE mymod.i PROPERTY CPLUSPLUS ON)
	# swig_add_library(mymod LANGUAGE python SOURCES mymod.i)
	
	set_property(SOURCE PySTARE1.i PROPERTY CPLUSPLUS ON)
	# swig_add_library(mymod LANGUAGE python SOURCES mymod.i)

    swig_add_library( PySTARE1 TYPE SHARED LANGUAGE python SOURCES PySTARE1.i )
    swig_link_libraries( PySTARE1 STARE )
    swig_link_libraries( PySTARE1 PySTARE1A )
    swig_link_libraries( PySTARE1 ${PYTHON_LIBRARIES} )
endif()

install(TARGETS STARE DESTINATION lib)
