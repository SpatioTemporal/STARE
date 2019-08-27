# CMakeLists.txt
# Created on: Jan 30, 2019
# Author: mrilee

#If we use the single-file versions of ERFA we won't need to search for the lib
#find_library( ERFA_LIB erfa )

# Possible future apps
# intersect, lookup, tesselateTrapezoid

add_executable              ( lowlevel-lookup-tool lowlevel-lookup-tool.cpp )
target_link_libraries       ( lowlevel-lookup-tool STARE )
target_include_directories  ( lowlevel-lookup-tool PUBLIC /usr/include )

add_executable              ( lookupSpatial lookupSpatial.cpp )
target_link_libraries       ( lookupSpatial STARE )
target_include_directories  ( lookupSpatial PUBLIC /usr/include )

add_executable              ( lookupTemporal lookupTemporal.cpp )
target_link_libraries       ( lookupTemporal STARE )
target_include_directories  ( lookupTemporal PUBLIC /usr/include )
