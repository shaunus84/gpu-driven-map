cmake_minimum_required(VERSION 3.21)
project(gpu-driven-map LANGUAGES CXX)

set(TARGET_NAME gpu-driven-map)

include(${CMAKE_SOURCE_DIR}/cmake/ProjectSetup.cmake)

# --- Sources ---
file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(${TARGET_NAME}
  ${GAME_SRC}
  ${GAME_HEADERS}   # headers for IDE convenience
)

target_include_directories(${TARGET_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(SDL2 CONFIG REQUIRED)
find_package(bgfx CONFIG REQUIRED)
find_package(Jolt CONFIG REQUIRED)

if(TARGET SDL2::SDL2)
  set(SDL2_TARGET SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
  set(SDL2_TARGET SDL2::SDL2-static)
else()
  message(FATAL_ERROR "Neither SDL2::SDL2 nor SDL2::SDL2-static found!")
endif()


# --- Link ---
target_link_libraries(${TARGET_NAME}
  PRIVATE
    culprit::Maths
    ${SDL2_TARGET}
    Jolt::Jolt
)

if (WIN32)
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
      $<TARGET_FILE_DIR:${TARGET_NAME}>
    COMMAND_EXPAND_LISTS
  )
endif()

foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER ${CONFIG} CONFIG_UPPER)
  set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER}
      ${CMAKE_SOURCE_DIR}/bin/${TARGET_NAME}
  )
endforeach()

# --- Platform niceties ---
if(MSVC)
  target_compile_definitions(${TARGET_NAME} PRIVATE SDL_MAIN_HANDLED)
endif()

if(APPLE)

  # We are NOT building an .app bundle yet
  set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE NO
    BUILD_RPATH "@loader_path"
  )

endif()

# IDE grouping
culprit_group_folders()
