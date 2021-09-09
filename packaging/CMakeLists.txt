
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (NOT DEFINED STARE_INSTALL_CMAKEDIR)
   set(STARE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/STARE"
       CACHE STRING "Path to STARE CMake files")
endif ()

if (NOT DEFINED STARE_INSTALL_LIBDIR)
   set(STARE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}"
       CACHE STRING "Path to library files")
endif()

# message( "STARE_INSTALL_CMAKEDIR: ${STARE_INSTALL_CMAKEDIR}" )
# message( "STARE_INSTALL_LIBDIR:   ${STARE_INSTALL_LIBDIR}" )
# message( "CMAKE_INSTALL_LIBDIR:   ${CMAKE_INSTALL_LIBDIR}" )

install(TARGETS STARE EXPORT STARE_Targets
        RUNTIME COMPONENT STARE_Runtime
        LIBRARY DESTINATION "${STARE_INSTALL_LIBDIR}" COMPONENT STARE_Runtime NAMELINK_COMPONENT STARE_Development
        ARCHIVE DESTINATION "${STARE_INSTALL_LIBDIR}" COMPONENT STARE_Development
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/STARE")

install(DIRECTORY "${STARE_BINARY_DIR}/src/include/STARE"
        TYPE INCLUDE
        COMPONENT STARE_Development)

if (BUILD_SHARED_LIBS)
    set(type shared)
else ()
    set(type static)
endif ()

install(EXPORT STARE_Targets
        DESTINATION "${STARE_INSTALL_CMAKEDIR}"
        NAMESPACE STARE::
        FILE STARE-${type}-targets.cmake
        COMPONENT STARE_Development)

write_basic_package_version_file(
    STAREConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/STAREConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/STAREConfigVersion.cmake"
        DESTINATION "${STARE_INSTALL_CMAKEDIR}"
        COMPONENT STARE_Development)

