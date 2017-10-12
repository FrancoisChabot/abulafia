cmake_minimum_required(VERSION 3.1)

project(abulafia)

option(ABULAFIA_BUILD_TESTS "Build the abulafia tests" ON)
option(ABULAFIA_BUILD_TESTS "Build the abulafia examples" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /W4 /wd4127")
  set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
  
  #Gtest workaround for MSVC2017
  add_definitions(-DGTEST_LANG_CXX11)
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -std=c++17 -ftemplate-backtrace-limit=0")
endif(MSVC)

set(INSTALL_GTEST OFF CACHE BOOL "Do not install gtest" FORCE)
set(INSTALL_GMOCK OFF CACHE BOOL "Do not install gmock" FORCE)

add_subdirectory(third_party/googletest)

set_target_properties(gmock PROPERTIES FOLDER "gtest")
set_target_properties(gmock_main PROPERTIES FOLDER "gtest")
set_target_properties(gtest PROPERTIES FOLDER "gtest")
set_target_properties(gtest_main PROPERTIES FOLDER "gtest")


# List of abulafia headers
FILE(GLOB_RECURSE
  ABULAFIA_HEADERS_GLOBBED
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

SET(ABULAFIA_HEADERS "")
FOREACH(f ${ABULAFIA_HEADERS_GLOBBED})
  IF(NOT ${f} MATCHES "abulafia_all.h")
    LIST(APPEND ABULAFIA_HEADERS "${f}")
  ENDIF()
ENDFOREACH(f)

SET(HEADER_PATHS "")
FOREACH(f ${ABULAFIA_HEADERS})
  LIST(APPEND HEADER_PATHS "include/${f}")
ENDFOREACH(f)

include_directories(
  ./include
  ${gtest_SOURCE_DIR}/include
  )

if(ABULAFIA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

SET(ALL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/abulafia/abulafia_all.h)
SET(NOTICE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/misc/notice.txt)

add_custom_command(
  OUTPUT ${ALL_FILE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
  DEPENDS ${HEADER_PATHS} 
  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/third_party/header_compiler/header_compiler.py 
    --output=${ALL_FILE}
    --notice=${NOTICE_FILE}
    --header_guard="ABULAFIA_SINGLE_INCLUDE_H_"
    --input=${CMAKE_CURRENT_SOURCE_DIR}/include/abulafia/abulafia.h)

add_custom_target(generate_all_include ALL DEPENDS ${ALL_FILE})
add_subdirectory(examples)