cmake_minimum_required(VERSION 3.20)
project(culprit-tools-shared LANGUAGES CXX VERSION 1.0.0)

add_library(culprit-tools-shared INTERFACE)
add_library(culprit::tools::Shared ALIAS culprit-tools-shared)

target_include_directories(culprit-tools-shared INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(culprit-tools-shared INTERFACE cxx_std_20)

file(GLOB_RECURSE SHARED_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
add_custom_target(culprit-tools-shared-headers SOURCES ${SHARED_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" FILES ${SHARED_HEADERS})

message(STATUS "[culprit-tools-shared] Target 'culprit::tools::Shared' ready for use.")