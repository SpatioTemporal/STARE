
set (
  STARE_SrcFiles
  ${CMAKE_CURRENT_SOURCE_DIR}/BitField.h
  ${CMAKE_CURRENT_SOURCE_DIR}/BitShiftNameEncoding.h
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/Convexio.h
  ${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedLevelNameEncoding.h
  ${CMAKE_CURRENT_SOURCE_DIR}/General.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HtmRange.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HtmRangeIterator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HtmRangeMultiLevel.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HtmRangeMultiLevelIterator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Htmio.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HstmRange.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HTMSubTreeNode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HTMSubTree.h
  ${CMAKE_CURRENT_SOURCE_DIR}/NameEncoding.h
  ${CMAKE_CURRENT_SOURCE_DIR}/PySTARE.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RangeConvex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RangeConvex.hxx
  ${CMAKE_CURRENT_SOURCE_DIR}/SkipList.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SkipListElement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialConstraint.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialConstraint.hxx
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialConvex.hxx
  #${CMAKE_CURRENT_SOURCE_DIR}/SpatialDoc.h		Is this file no longer needed here? -kln 4/25/19
  #${CMAKE_CURRENT_SOURCE_DIR}/SpatialDomain.bak_h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialDomain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialDomain.hxx
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialEdge.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialException.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialGeneral.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialIndex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialIndex.hxx
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialInterface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialInterface.hxx
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialPolygon.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialSign.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialVector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialVector.hxx
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialRange.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpatialRotation.h
  ${CMAKE_CURRENT_SOURCE_DIR}/STARE.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TemporalGeneral.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TemporalIndex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TemporalWordFormat1.h	
  ${CMAKE_CURRENT_SOURCE_DIR}/VarCmp.h
  ${CMAKE_CURRENT_SOURCE_DIR}/VarStr.h
  ${CMAKE_CURRENT_SOURCE_DIR}/VarStr.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VarStr.hxx )

# TODO Trouble with VarStr.hpp
# Later...
# add_library(HEADER_ONLY_TARGET STATIC test1.hpp test2.hpp)
# set_target_properties(HEADER_ONLY_TARGET PROPERTIES LINKER_LANGUAGE CXX)
#
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects
#

install(FILES STARE.h
  BitField.h
  BitShiftNameEncoding.h
  CMakeLists.txt
  Convexio.h
  EmbeddedLevelNameEncoding.h
  General.h
  HtmRange.h
  HtmRangeIterator.h
  HtmRangeMultiLevel.h
  HtmRangeMultiLevelIterator.h
  Htmio.h
  HstmRange.h
  HTMSubTreeNode.h
  HTMSubTree.h
  KeyPair.h
  NameEncoding.h
  RangeConvex.h
  RangeConvex.hxx
  SkipList.h
  SkipListElement.h
  SpatialConstraint.h
  SpatialConstraint.hxx
  SpatialConvex.hxx
  SpatialDomain.h
  SpatialDomain.hxx
  SpatialEdge.h
  SpatialException.h
  SpatialGeneral.h
  SpatialIndex.h
  SpatialIndex.hxx
  SpatialInterface.h
  SpatialInterface.hxx
  SpatialPolygon.h
  SpatialRange.h
  SpatialSign.h
  SpatialVector.h
  SpatialVector.hxx
  SpatialRotation.h
  TemporalGeneral.h
  TemporalIndex.h
  TemporalIndex1.h
  TemporalWordFormat1.h	
  VarCmp.h
  VarStr.h
  VarStr.hpp
  VarStr.hxx 
  ../external/erfa-single-file/erfa.h DESTINATION include)
