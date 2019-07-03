
set (
  TestSrcFiles
  Test.cpp
  SpatialFailure_test.cpp
  SpatialIndex_test.cpp
  SpatialRotation_test.cpp
  SpatialVector_test.cpp
  STARE_test.cpp
  TemporalIndex_test.cpp
  Test.h 
  )

# SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -I${PROJECT_SOURCE_DIR}/include" )

add_executable(tests ${TestSrcFiles})
target_link_libraries(tests STARE)

# target_include_directories(tests PUBLIC ${CUTE_INCLUDE_DIR}/cute /usr/include)
# ${CUTE_INCLUDE_DIR} is the directory that holds the CUTE library
# headers, adding a subdir to it is not needed. jhrg 5/1/19
# target_include_directories(tests PUBLIC ${CUTE_INCLUDE_DIR}/cute)

target_include_directories(tests PUBLIC ${CUTE_INCLUDE_DIR} )

add_test( STARETests tests )

set_tests_properties( STARETests PROPERTIES
  PASS_REGULAR_EXPRESSION "#success;#starting"
  FAIL_REGULAR_EXPRESSION "#failure;#error"
)
