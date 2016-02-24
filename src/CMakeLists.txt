
set (
  HSTMSrcFiles
  ${HSTMSrcFiles}
  BitShiftNameEncoding.C
  EmbeddedLevelNameEncoding.C
  HtmRange.cpp
  HtmRangeIterator.cpp
  Htmio.cpp
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
  SpatialVector.cpp
  VarStr.C )
  
# TODO Trouble with VarStr.hpp# TODO Trouble with VarStr.hpp
# Var.Str.C is a kluge because I don't know how to get CMake to handle the hpp files correctly.
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects

add_library( hstm STATIC ${HSTMSrcFiles} )
