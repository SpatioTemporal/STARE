
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
  HTMSubTreeNode.cpp
  HTMSubTree.cpp
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
# Var.Str.cpp is a kluge because I don't know how to get CMake to handle the hpp files correctly.
# INTERFACE May be what works: http://stackoverflow.com/questions/5957134/how-to-setup-cmake-to-generate-header-only-projects

# add_library( STARE STATIC ${STARE_SrcFiles} )

add_library( STARE ${STARE_SrcFiles} )
add_library( STARE::STARE ALIAS STARE )
set_target_properties(STARE PROPERTIES
			    VERSION "${STARE_VERSION}"
			    SOVERSION "${STARE_VERSION_MAJOR}"
			    POSITION_INDEPENDENT_CODE "${BUILD_SHARED_LIBS}")

include(GenerateExportHeader)
generate_export_header(STARE EXPORT_FILE_NAME include/STARE/export.h)
target_compile_definitions(
	STARE PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:STARE_STATIC_DEFINE>")

target_include_directories(
	STARE
	PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/STARE>"
	PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	)
	
target_compile_features(STARE PUBLIC cxx_std_11)

# if(not STARE_INCLUDE_PACKAGING)
#   install(TARGETS STARE DESTINATION lib)
# endif()
