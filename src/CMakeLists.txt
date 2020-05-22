
set (
  STARE_SrcFiles
  ${STARE_SrcFiles}
  BitShiftNameEncoding.cpp
  EmbeddedLevelNameEncoding.cpp
  General.cpp
  HtmRange.cpp
  HtmRangeIterator.cpp
  HtmRangeMultiLevel.cpp
  HtmRangeMultiLevelIterator.cpp
  Htmio.cpp
  HstmRange.cpp
  NameEncoding.cpp
  RangeConvex.cpp
  SkipList.cpp
  SkipListElement.cpp
  SpatialConstraint.cpp
  SpatialDomain.cpp
  SpatialEdge.cpp
  SpatialException.cpp
  SpatialGeneral.cpp
  SpatialIndex.cpp
  SpatialInterface.cpp
  SpatiallyAdaptiveDataCover.cpp
  SpatialVector.cpp
  SpatialPolygon.cpp
  SpatialRotation.cpp
  STARE.cpp
  SpatialRange.cpp
  TemporalIndex.cpp
  TemporalWordFormat1.cpp
  VarStr.cpp
  ../external/erfa-single-file/erfa.c )
  
# TODO Trouble with VarStr.hpp
# Var.Str.C is a kluge because I don't know how to get CMake to handle the hpp files correctly.
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects

add_library( STARE STATIC ${STARE_SrcFiles} )

install(TARGETS STARE DESTINATION lib)
