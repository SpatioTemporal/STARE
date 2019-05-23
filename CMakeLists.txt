
cmake_minimum_required(VERSION 3.4)

project(STARE)

option(CUTE_INCLUDE_DIR "Location of the CUTE headers." ON )

if(DEFINED ENV{CUTE_INCLUDE_DIR})
	set( CUTE_INCLUDE_DIR $ENV{CUTE_INCLUDE_DIR} )
endif()

# If an environment variable was not set, then guess.
if(CUTE_INCLUDE_DIR MATCHES "ON")
    set( CUTE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/cute )
endif()

message("\n------------ Running cmake for project STARE ------------\n")

# Intentionally fail to test. set( CUTE_INCLUDE_DIR "/dev/none")
if(EXISTS "${CUTE_INCLUDE_DIR}")
    message( " CUTE_INCLUDE_DIR: ${CUTE_INCLUDE_DIR}\n ")
    include(CTest)
    message( " CUTE Testing Enabled\n" )
else()
     # message( FATAL_ERROR " CUTE_INCLUDE_DIR not found! ${CUTE_INCLUDE_DIR}\n ")
     message( WARNING " CUTE_INCLUDE_DIR not set, using ${CUTE_INCLUDE_DIR}\nContinuing without configuring tests...\n")
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

#file(GLOB SOURCES "/src/")
#add_library( STARE STATIC ${SOURCES} )

# Read the version number from a file.
file (STRINGS "version.txt" VERSION)

# Generate the header in the source include dir. Not really the
# best plan since it should be in the build dir, but I cannot get
# that working and this is a version number and thus not platform
# specific. jhrg 5/22/19
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/STARE.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/include/STARE.h @ONLY)

# This command does not work
# target_include_directories(STARE ${CMAKE_CURRENT_BINARY_DIR}/include)

# These commands replicate automake's 'make dist'.
# https://agateau.com/2009/cmake-and-make-dist/. jhrg 5/20/19
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")

set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)

# By default, CPack names the target 'package_source'. This makes
# a second target named 'dist' to match the behavior of automake.
# jhrg 5/20/19
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

