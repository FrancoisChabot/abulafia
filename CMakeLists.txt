cmake_minimum_required(VERSION 3.1)

project(abulafia)

option(ABULAFIA_BUILD_TESTS "Build the abulafia tests" ON)
option(ABULAFIA_BUILD_TESTS "Build the abulafia examples" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4 /wd4127")
  set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
  
  #Gtest workaround for MSVC2017
  add_definitions(-DGTEST_LANG_CXX11)
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -std=c++17 --coverage")
endif(MSVC)

set(INSTALL_GTEST OFF CACHE BOOL "Do not install gtest" FORCE)
set(INSTALL_GMOCK OFF CACHE BOOL "Do not install gmock" FORCE)

add_subdirectory(googletest)


# List of abulafia headers
SET(ABULAFIA_HEADERS
  abulafia/char_set/any.h
  abulafia/char_set/char_set.h
  abulafia/char_set/delegated.h
  abulafia/char_set/not.h
  abulafia/char_set/operators.h
  abulafia/char_set/or.h
  abulafia/char_set/range.h
  abulafia/char_set/set.h
  abulafia/char_set/single.h

  abulafia/contexts/container_sequence.h
  abulafia/contexts/single_forward.h
  abulafia/contexts/skipper_adapter.h

  abulafia/operations/parse.h

  abulafia/patterns/binary/except.h
  abulafia/patterns/binary/list.h

  abulafia/patterns/helpers/buffer.h
  abulafia/patterns/helpers/collection_assign_wrapper.h

  abulafia/patterns/leaf/numeric/digit_values.h
  abulafia/patterns/leaf/numeric/int.h
  abulafia/patterns/leaf/numeric/uint.h
  abulafia/patterns/leaf/char_literal.h
  abulafia/patterns/leaf/char_symbol.h
  abulafia/patterns/leaf/character.h
  abulafia/patterns/leaf/eoi.h
  abulafia/patterns/leaf/fail.h
  abulafia/patterns/leaf/pass.h
  abulafia/patterns/leaf/string_literal.h
  abulafia/patterns/leaf/string_symbol.h

  abulafia/patterns/nary/alternative.h
  abulafia/patterns/nary/nary.h
  abulafia/patterns/nary/sequence.h

  abulafia/patterns/unary/action.h
  abulafia/patterns/unary/attr_cast.h
  abulafia/patterns/unary/not.h
  abulafia/patterns/unary/repeat.h
  abulafia/patterns/operators.h
  abulafia/patterns/recur.h
  abulafia/patterns/with_skipper.h

  abulafia/support/assert.h
  abulafia/support/function_traits.h
  abulafia/support/nil.h
  abulafia/support/type_traits.h
  abulafia/support/variant_utils.h
  abulafia/support/visit_val.h

  abulafia/abulafia.h
  abulafia/config.h
  abulafia/parser.h
  abulafia/pattern.h
  abulafia/patterns.h
  abulafia/result.h
  abulafia/traits.h
)


include_directories(
  ./include
  ${gtest_SOURCE_DIR}/include
  )

if(ABULAFIA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
