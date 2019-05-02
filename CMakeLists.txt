
cmake_minimum_required(VERSION 3.4)

project(STARE)

option(CUTE_INCLUDE_DIR "Location of the CUTE headers." ON )

if(DEFINED ENV{CUTE_INCLUDE_DIR})
	set( CUTE_INCLUDE_DIR $ENV{CUTE_INCLUDE_DIR} )
endif()

# If an environment variable was not set, then guess.
if(CUTE_INCLUDE_DIR MATCHES "ON")
    set( CUTE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/CUTE_DIR/CUTE )
endif()

message("\n------------ Running cmake for project STARE ------------\n")
message("\n Cute Dir: ${CUTE_INCLUDE_DIR} \n Source Dir: ${PROJECT_SOURCE_DIR}")

# Intentionally fail to test. set( CUTE_INCLUDE_DIR "/dev/none")
if(EXISTS "${CUTE_INCLUDE_DIR}")
    message( " CUTE_INCLUDE_DIR: ${CUTE_INCLUDE_DIR}\n ")
    include(CTest)
    message( " CUTE Testing Enabled\n" )
else()
     # message( FATAL_ERROR " CUTE_INCLUDE_DIR not found! ${CUTE_INCLUDE_DIR}\n ")
     message( WARNING " CUTE_INCLUDE_DIR not found! ${CUTE_INCLUDE_DIR}\nContinuing without configuring tests...\n")
endif()

# Set CC and CXX before calling cmake...
# set(CMAKE_C_COMPILER gcc)
# set(CMAKE_CXX_COMPILER g++)
 
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
     set(CMAKE_C_FLAGS "-g -Wall -D_BOOL_EXISTS -D__macosx -UDIAGNOSE -fPIC ${CMAKE_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -D_BOOL_EXISTS -D__macosx -UDIAGNOSE -fPIC ${CMAKE_CXX_FLAGS}")
elseif(UNIX)
     set(CMAKE_C_FLAGS "-g -Wall -D_BOOL_EXISTS -D__unix -UDIAGNOSE -fPIC ${CMAKE_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -D_BOOL_EXISTS -D__unix -UDIAGNOSE -fPIC ${CMAKE_CXX_FLAGS}")
else()
     message(FATAL_ERROR " Platform ${CMAKE_SYSTEM_NAME} not handled.")
endif()

#add_subdirectory is used when the subdirectory has its own CMakeLists.txt
# Otherwise, include_directories is used to access files in the directory
include_directories(${PROJECT_SOURCE_DIR}/include )
add_subdirectory(${PROJECT_SOURCE_DIR}/include )
if(BUILD_TESTING)
  message( "Adding tests...\n" )
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests/CUTE)
endif()
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/app)

#target_include_directories(hstm PUBLIC ${CUTE_INCLUDE_DIR}/cute)

#file(GLOB SOURCES "/src/")
#add_library( STARE STATIC ${SOURCES} )

