
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
  STARE_test.cpp
  STARE_Covers_test.cpp
  HTMSubTree_test.cpp
  TemporalIndex_test.cpp
  Test.cpp
  )
  
add_executable(tests ${TestSrcFiles})
target_link_libraries(tests STARE)

target_include_directories(tests PUBLIC ${CUTE_INCLUDE_DIR} )

add_test( STARETests tests )

set_tests_properties( STARETests PROPERTIES
  PASS_REGULAR_EXPRESSION "#success;#starting"
  FAIL_REGULAR_EXPRESSION "#failure;#error"
  ENVIRONMENT "STARE_SRC_HOME=${CMAKE_SOURCE_DIR}"
)

# Copy files so that one car run <build-directory>/tests/CUTE/tests
# from command line.
file(COPY polygons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
