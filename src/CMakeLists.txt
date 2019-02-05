
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
