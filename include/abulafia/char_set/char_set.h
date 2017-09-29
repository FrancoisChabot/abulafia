//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_CHAR_SET_H_
#define ABULAFIA_CHAR_SET_CHAR_SET_H_

#include "abulafia/config.h"

#include <type_traits>

namespace ABULAFIA_NAMESPACE {
namespace char_set {

struct CharacterSet {};

template <typename T>
struct is_char_set : public std::is_base_of<CharacterSet, T> {};


template<typename T, typename Enable=void>
struct to_char_set_impl;

template<typename T>
struct to_char_set_impl<T, std::enable_if_t<is_char_set<T>::value>> {
  static T const& convert(T const& v) {
    return v;
  }
};

template<typename T>
auto to_char_set(T v) {
  return to_char_set_impl<T>::convert(v);
}

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif