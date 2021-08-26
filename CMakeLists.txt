cmake_minimum_required(VERSION 3.16)
project(abulafia)

enable_testing()

include(cmake/code-coverage.cmake)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)

# Single include baking
SET(ALL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/abulafia/abulafia_all.h)

add_custom_command(
  OUTPUT ${ALL_FILE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
  DEPENDS ${GLOB_OF_ALL_HEADERS} 
  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/misc/header_compiler.py 
    --output=${ALL_FILE}
    --notice=${CMAKE_CURRENT_SOURCE_DIR}/misc/notice.txt
    --header_guard="MY_PROJECT_HEADER_GUARD_H_"
    --input=${CMAKE_CURRENT_SOURCE_DIR}/include/abulafia/abulafia.h)
add_custom_target(generate_all_include ALL DEPENDS ${ALL_FILE})


add_library(abulafia INTERFACE)
target_include_directories(abulafia INTERFACE include)
target_compile_features(abulafia INTERFACE cxx_std_20)

add_subdirectory(tests)