
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
  
# TODO Trouble with VarStr.hpp
# Var.Str.C is a kluge because I don't know how to get CMake to handle the hpp files correctly.
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects

add_library( STARE STATIC ${STARE_SrcFiles} )

# FIXME jhrg 5/1/19 Fails for me. find_package(Boost)
set ( Boost_FOUND  OFF )

if(Boost_FOUND)
  include_directories( "${Boost_INCLUDE_DIRS}" "/usr/include/python3.7m"  )
  set(Boost_USE_STATIC_LIBS OFF)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  find_package(Boost COMPONENTS python3 COMPONENTS numpy3 REQUIRED)

  set( STARE_PYTHON_SrcFiles
	PySTARE.cpp
  )
  add_library( PySTARE SHARED ${STARE_SrcFiles} ${STARE_PYTHON_SrcFiles} )
  set_target_properties( PySTARE PROPERTIES PREFIX "" )
  target_link_libraries( PySTARE ${Boost_LIBRARIES} )
else()
  message("Boost not found, not creating shared object PySTARE, including the python 3 API. Was BOOST_ROOT set?\nContinuing...\n")  
endif()

install(TARGETS STARE DESTINATION lib)
