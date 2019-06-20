
set (
  STARE_SrcFiles
  ${STARE_SrcFiles}
  BitShiftNameEncoding.C
  EmbeddedLevelNameEncoding.C
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
  VarStr.C )
  
# TODO Trouble with VarStr.hpp# TODO Trouble with VarStr.hpp
# Var.Str.C is a kluge because I don't know how to get CMake to handle the hpp files correctly.
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects

add_library( STARE STATIC ${STARE_SrcFiles} )
set(BOOST_ROOT "/usr/include/boost169")

# Boost is deprecated
find_package(Boost)
if(Boost_FOUND)
  message("${Boost_INCLUDE_DIRS}")
  include_directories( "${Boost_INCLUDE_DIRS}" "/usr/include/python3.6m" "/usr/include/python3.7m" )
  set(Boost_USE_STATIC_LIBS OFF)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  find_package(Boost COMPONENTS python3 COMPONENTS numpy3 REQUIRED)

  set( STARE_PYTHON_SrcFiles
	PySTARE.cpp
  )
  add_library( PySTARE SHARED ${STARE_SrcFiles} ${STARE_PYTHON_SrcFiles} )
  set_target_properties( PySTARE PROPERTIES PREFIX "" )
  target_link_libraries( PySTARE ${Boost_LIBRARIES} "/usr/lib64/liberfa.so" )
else()
  message("Boost not found, not creating shared object PySTARE, including the python 3 API. Was BOOST_ROOT set?\nContinuing...\n")  
endif()

# SWIG integration of PySTARE1
if(true)
	# UseSWIG_TARGET_NAME_PREFERENCE
	cmake_policy( SET CMP0078 NEW )
	cmake_policy( SET CMP0086 NEW )

    include_directories( "/usr/include/python3.7m"  )
    add_library(PySTARE1A SHARED PySTARE1.cpp)
    # set_target_properties( PySTARE1 PROPERTIES PREFIX "" )
    #? set target properties(PySTARE1 PROPERTIES LIBRARY OUTPUT DIRECTORY ${PROJECT BINARY DIR}/..) 
    target_link_libraries( PySTARE1A "/usr/lib64/liberfa.so" )
    
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

