
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

find_package(SWIG)
# SWIG integration of PySTARE
if(SWIG_FOUND)
	# UseSWIG_TARGET_NAME_PREFERENCE
	cmake_policy( SET CMP0078 NEW )
	cmake_policy( SET CMP0086 NEW )
	
	# TODO Control following with ENV variable.
    include_directories( "/usr/include/python3.7m"  "/usr/lib64/python3.7/site-packages/numpy/core/include"  )
    add_library(PySTAREA SHARED PySTARE.cpp)
    # set_target_properties( PySTARE PROPERTIES PREFIX "" )
    #? set target properties(PySTARE PROPERTIES LIBRARY OUTPUT DIRECTORY ${PROJECT BINARY DIR}/..) 
    target_link_libraries( PySTAREA "/usr/lib64/liberfa.so" )
    
    # SWIG
    find_package(SWIG REQUIRED)
    include(${SWIG_USE_FILE})
    find_package(PythonLibs)
    include_directories(${PYTHON_INCLUDE_DIRS})
    
    # set_property(SOURCE mymod.i PROPERTY CPLUSPLUS ON)
	# swig_add_library(mymod LANGUAGE python SOURCES mymod.i)
	
	set_property(SOURCE PySTARE.i PROPERTY CPLUSPLUS ON)
	# swig_add_library(mymod LANGUAGE python SOURCES mymod.i)

    swig_add_library( PySTARE TYPE SHARED LANGUAGE python SOURCES PySTARE.i )
    swig_link_libraries( PySTARE STARE )
    swig_link_libraries( PySTARE PySTAREA )
    swig_link_libraries( PySTARE ${PYTHON_LIBRARIES} )
endif()

install(TARGETS STARE DESTINATION lib)
