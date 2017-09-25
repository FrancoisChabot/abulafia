cmake_minimum_required(VERSION 3.1)

project(abulafia)

option(ABULAFIA_BUILD_TESTS "Build the abulafia tests" ON)
option(ABULAFIA_BUILD_TESTS "Build the abulafia examples" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4 /wd4127")
  set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
  
  #Gtest workaround for MSVC2017
  add_definitions(-DGTEST_LANG_CXX11)
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif(MSVC)

set(INSTALL_GTEST OFF CACHE BOOL "Do not install gtest" FORCE)
set(INSTALL_GMOCK OFF CACHE BOOL "Do not install gmock" FORCE)

add_subdirectory(googletest)

include_directories(
  ./include
  ${gtest_SOURCE_DIR}/include
  )

if(ABULAFIA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
