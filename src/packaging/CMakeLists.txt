
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (NOT DEFINED STARE_INSTALL_CMAKEDIR)
   set(STARE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/STARE"
       CACHE STRING "Path to STARE CMake files")
endif ()


install(TARGETS STARE EXPORT STARE_Targets
        RUNTIME COMPONENT STARE_Runtime
        LIBRARY COMPONENT STARE_Runtime
        NAMELINK_COMPONENT STARE_Development
        ARCHIVE COMPONENT STARE_Development
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${STARE_SOURCE_DIR}/include/" "${STARE_BINARY_DIR}/include/"
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

