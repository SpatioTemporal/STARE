
# CMakeLists.txt
# Author: Michael Rilee, mike@rilee.net, Rilee Systems Technologies LLC
# Date:   c.2019

set (
  TestSrcFiles
  Test.h 
  EmbeddedLevelNameEncoding_test.cpp
  AdaptSpatialResolutionEstimates_test.cpp
  SpatialFailure_test.cpp
  SpatialIndex_test.cpp
  SpatialInterface_test.cpp
  SpatialPolygon_test.cpp
  SpatialRange_test.cpp
  SpatialRotation_test.cpp
  SpatialVector_test.cpp
  SpatioTemporalUsage_test.cpp
  STARE_test.cpp
  STARE_Covers_test.cpp
  HTMSubTree_test.cpp
  TemporalIndex_test.cpp
  Test.cpp
  )


add_executable(tests ${TestSrcFiles})

set_property(TARGET tests PROPERTY POSITION_INDEPENDENT_CODE 1 )

target_link_directories(tests PUBLIC ${CMAKE_BINARY_DIR}/src)

# if(BUILD_SHARED_LIBS)
#   set( STARE_LINK_LIBS "libSTARE${CMAKE_SHARED_LIBRARY_SUFFIX}" )
# else()
#   set( STARE_LINK_LIBS "${CMAKE_BINARY_DIR}/src/libSTARE.a" )
# endif()

# SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# include_directories(${CMAKE_SOURCE_DIR}/include)
# link_directories(${CMAKE_BINARY_DIR}/src)

target_link_libraries(tests PRIVATE STARE::STARE)
# target_link_libraries(tests PUBLIC ${STARE_LINK_LIBS})

target_include_directories(tests
  PUBLIC ${CUTE_INCLUDE_DIR}
  PUBLIC ${CMAKE_BINARY_DIR}/src/include/STARE
  PUBLIC ${CMAKE_SOURCE_DIR}/include
  )

add_test( STARETests tests )

set_tests_properties( STARETests PROPERTIES
  PASS_REGULAR_EXPRESSION "#success;#starting"
  FAIL_REGULAR_EXPRESSION "#failure;#error"
  ENVIRONMENT "STARE_SRC_HOME=${CMAKE_SOURCE_DIR}"
)

# Copy files so that one car run <build-directory>/tests/CUTE/tests
# from command line.
file(COPY polygons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
