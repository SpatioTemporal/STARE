
cmake_minimum_required(VERSION 2.8)

project(STARE)

option(CUTE_INCLUDE_DIR "Location of the CUTE headers." ON )

if(DEFINED ENV{CUTE_INCLUDE_DIR})
    set( CUTE_INCLUDE_DIR $ENV{CUTE_INCLUDE_DIR} )
endif()

# If an environment variable was not set, then guess.
if(CUTE_INCLUDE_DIR MATCHES "ON")
    set( CUTE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/external/CUTE/cute )
endif()

set ( ERFA_DIR ${PROJECT_SOURCE_DIR}/external/erfa-single-file )

message("\n------------ Running cmake for project STARE ------------\n")

# Intentionally fail to test. set( CUTE_INCLUDE_DIR "/dev/none")
if(EXISTS "${CUTE_INCLUDE_DIR}")
    message( " CUTE_INCLUDE_DIR: ${CUTE_INCLUDE_DIR}\n ")
    include(CTest)
    enable_testing()
    message( " CUTE Testing Enabled\n" )
else()
     # message( FATAL_ERROR " CUTE_INCLUDE_DIR not found! ${CUTE_INCLUDE_DIR}\n ")
     message( WARNING " CUTE_INCLUDE_DIR not set, using ${CUTE_INCLUDE_DIR}\nContinuing without configuring tests...\n")
endif()

# Set CC and CXX before calling cmake...
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

if( NOT DEFINED OPTFLAGS )
  if(DEFINED ENV{OPTFLAGS})
    set(C_OPT_FLAGS   "$ENV{OPTFLAGS}")
    set(CXX_OPT_FLAGS "$ENV{OPTFLAGS}")
  else()
    message( "Setting C_OPT_FLAGS, CXX_OPT_FLAGS to -O3 by default.\n" )
    set(C_OPT_FLAGS "-O3")
    set(CXX_OPT_FLAGS "-O3")
  endif()
else()
  message( "Setting C_OPT_FLAGS, CXX_OPT_FLAGS to OPTFLAGS='${OPTFLAGS}'.\n" )
  set(C_OPT_FLAGS   "${OPTFLAGS}")
  set(CXX_OPT_FLAGS "${OPTFLAGS}")
endif()
message("Optimization settings.")
message("C_OPT_FLAGS:  " ${C_OPT_FLAGS})
message("CXX_OPT_FLAGS " ${CXX_OPT_FLAGS} "\n")
 
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
     set(CMAKE_C_FLAGS "-g ${C_OPT_FLAGS} -Wall -D_BOOL_EXISTS -UDIAGNOSE -fPIC ${CMAKE_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "-std=c++11 -g ${CXX_OPT_FLAGS} -Wall -D_BOOL_EXISTS -UDIAGNOSE -fPIC ${CMAKE_CXX_FLAGS}")
elseif(UNIX)
     set(CMAKE_C_FLAGS "-g ${C_OPT_FLAGS} -Wall -D_BOOL_EXISTS -D__unix -UDIAGNOSE -fPIC ${CMAKE_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "-std=c++11 -g ${CXX_OPT_FLAGS} -Wall -D_BOOL_EXISTS -D__unix -UDIAGNOSE -fPIC ${CMAKE_CXX_FLAGS}")
else()
     message(FATAL_ERROR " Platform ${CMAKE_SYSTEM_NAME} not handled.")
endif()

#add_subdirectory is used when the subdirectory has its own CMakeLists.txt
# Otherwise, include_directories is used to access files in the directory
# include_directories( ${PROJECT_SOURCE_DIR}/include /usr/include )
include_directories(${ERFA_DIR} )
include_directories(${PROJECT_SOURCE_DIR}/include )
add_subdirectory(${PROJECT_SOURCE_DIR}/include )

if(BUILD_TESTING)
  message( "Adding tests...\n" )
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests/CUTE)
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests/python)
endif()
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/app)
add_subdirectory(${PROJECT_SOURCE_DIR}/documentation )


#file(GLOB SOURCES "/src/")
#add_library( STARE STATIC ${SOURCES} )

# Read the version number from a file.
file (STRINGS "VERSION" VERSION)

# Generate the header in the source include dir. Not really the
# best plan since it should be in the build dir, but I cannot get
# that working and this is a version number and thus not platform
# specific. jhrg 5/22/19
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/STARE.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/include/STARE.h @ONLY)

# This will put the generated header in the directory where cmake is
# run. That would be great, but I have no idea how to get cmake to
# tell the compiler to look there for headers.
# configure_file(include/STARE.h.in STARE.h @ONLY)
# This command does not work. jhrg 5/23/19
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

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
