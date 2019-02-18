
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
  VarStr.C )
  
# TODO Trouble with VarStr.hpp# TODO Trouble with VarStr.hpp
# Var.Str.C is a kluge because I don't know how to get CMake to handle the hpp files correctly.
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects

add_library( STARE STATIC ${STARE_SrcFiles} )

find_package(Boost)
if(Boost_FOUND)
  # include_directories( "${Boost_INCLUDE_DIRS}" "/usr/include/python2.7"  )
  include_directories( "${Boost_INCLUDE_DIRS}" "/usr/include/python3.7m"  )
  set(Boost_USE_STATIC_LIBS OFF)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  find_package(Boost COMPONENTS python3 COMPONENTS numpy3 REQUIRED)

  set( STARE_PYTHON_SrcFiles
	SSTARE.cpp
  )
  # add_library( SSTARE SHARED ${STARE_SrcFiles} ${STARE_PYTHON_SrcFiles} )
  add_library( SSTARE SHARED ${STARE_SrcFiles} ${STARE_PYTHON_SrcFiles} )
  target_link_libraries( SSTARE ${Boost_LIBRARIES} )  
else()
  message("Boost not found, not creating shared object SSTARE, including python API. Was BOOST_ROOT set?")  
endif()


