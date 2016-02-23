
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

add_library( hstm STATIC ${HSTMSrcFiles} )
