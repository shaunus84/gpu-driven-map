cmake_minimum_required(VERSION 3.21)

project(map-parser VERSION 1.0.0 LANGUAGES CXX)

set(TARGET_NAME map-parser)

include(${CMAKE_SOURCE_DIR}/cmake/ProjectSetup.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CulpritTools.cmake)

file(GLOB_RECURSE SRC_FILES    *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h *.hpp)

add_executable(${TARGET_NAME} ${SRC_FILES} ${HEADER_FILES})

# C++ standard + warnings
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
if (MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /permissive- /Zc:__cplusplus /utf-8 /MP)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(${TARGET_NAME} PRIVATE
  culprit::tools::Shared
  culprit::Maths
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    $<INSTALL_INTERFACE:include>                      # harmless for exe
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

culprit_group_folders()
culprit_setup_tool(${TARGET_NAME})
