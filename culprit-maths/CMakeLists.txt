cmake_minimum_required(VERSION 3.20)
project(culprit-maths VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

# Header-only library
add_library(culprit-maths INTERFACE)
add_library(culprit::Maths ALIAS culprit-maths)

target_include_directories(culprit-maths INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(culprit-maths INTERFACE cxx_std_20)

# (Optional) IDE convenience so headers show up in Solutions
file(GLOB_RECURSE MATHS_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
add_custom_target(culprit-maths-headers SOURCES ${MATHS_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" FILES ${MATHS_HEADERS})

# Export into the unified SDK export set
if(NOT DEFINED CULPRIT_EXPORT_SET)
  set(CULPRIT_EXPORT_SET culpritTargets)
endif()

install(TARGETS culprit-maths
  EXPORT ${CULPRIT_EXPORT_SET})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Optional: emit a standalone package if you ever build this in isolation
option(CULPRIT_MATHS_STANDALONE_PACKAGE "Generate standalone CulpritMaths package files" OFF)
if(CULPRIT_MATHS_STANDALONE_PACKAGE)
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CulpritMathsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CulpritMathsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CulpritMathsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CulpritMaths)
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CulpritMathsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CulpritMathsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CulpritMaths)
  install(EXPORT ${CULPRIT_EXPORT_SET}
    NAMESPACE culprit::
    FILE CulpritMathsTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CulpritMaths)
endif()

message(STATUS "[culprit-maths] Target 'culprit::Maths' ready for use.")
