# CMakeLists.txt
# Created on: Jan 30, 2019
# Author: mrilee

find_library( ERFA_LIB erfa )

add_executable              ( intersect intersect.cpp )
target_link_libraries       ( intersect STARE)
target_include_directories  ( intersect PUBLIC )

add_executable              ( lowlevel-lookup-tool lowlevel-lookup-tool.cpp )
target_link_libraries       ( lowlevel-lookup-tool STARE "${ERFA_LIB}" )
target_include_directories  ( lowlevel-lookup-tool PUBLIC /usr/include )

# add_executable              ( lookup lookup.cpp )
# target_link_libraries       ( lookup STARE)
# target_include_directories  ( lookup PUBLIC )

# Broken
add_executable              ( lookupInteractive lookupInteractive.cpp )
target_link_libraries       ( lookupInteractive STARE)
target_include_directories  ( lookupInteractive PUBLIC )

# Currently broken -- returns triangles associated with a trapezoid
add_executable              ( tesselateTrapezoid tesselateTrapezoid.cpp )
target_link_libraries       ( tesselateTrapezoid STARE)
target_include_directories  ( tesselateTrapezoid PUBLIC )

