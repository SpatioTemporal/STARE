
cmake_minimum_required(VERSION 3.4)

project(HSTM)

option(CUTE_INCLUDE_DIR "Location of the CUTE headers." ${PROJECT_SOURCE_DIR}/cute )

# Set CC and CXX before calling cmake...
# set(CMAKE_C_COMPILER gcc)
# set(CMAKE_CXX_COMPILER g++)


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
     set(CMAKE_C_FLAGS "-g -Wall -D_BOOL_EXISTS -D__macosx -UDIAGNOSE ${CMAKE_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -D_BOOL_EXISTS -D__macosx -UDIAGNOSE ${CMAKE_CXX_FLAGS}")
elseif(UNIX)
     set(CMAKE_C_FLAGS "-g -Wall -D_BOOL_EXISTS -D__unix -UDIAGNOSE ${CMAKE_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -D_BOOL_EXISTS -D__unix -UDIAGNOSE ${CMAKE_CXX_FLAGS}")
else()
     message(FATAL_ERROR " Platform ${CMAKE_SYSTEM_NAME} not handled.")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include )
include_directories(${CUTE_INCLUDE_DIR})

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests/CUTE)

