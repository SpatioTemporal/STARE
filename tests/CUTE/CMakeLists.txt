cmake_minimum_required(VERSION 3.4)

set (
  CuteTestsSrcFiles
  Test.cpp )

# SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -I${PROJECT_SOURCE_DIR}/include" )

add_executable(tests ${CuteTestsSrcFiles})
target_link_libraries(tests hstm)
target_include_directories(tests PUBLIC ${CUTE_INCLUDE_DIR}/cute)

