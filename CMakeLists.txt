
cmake_minimum_required(VERSION 3.4)

project(HSTM)

# Set CC and CXX before calling cmake...
# set(CMAKE_C_COMPILER gcc)
# set(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_FLAGS "-g -Wall -D_BOOL_EXISTS -D__macosx -UDIAGNOSE ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -D_BOOL_EXISTS -D__macosx -UDIAGNOSE ${CMAKE_CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR}/include )
include_directories(${PROJECT_SOURCE_DIR}/cute )

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests/CUTE)

