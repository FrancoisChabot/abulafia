//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_TYPE_TRAITS_H_
#define ABULAFIA_SUPPORT_TYPE_TRAITS_H_

#include "abulafia/config.h"

#include <tuple>
#include <type_traits>
#include <utility>

// Importing standard C++ trait and utilities directly in the abulafia
// namespace.
namespace ABULAFIA_NAMESPACE {
using std::decay_t;
using std::enable_if_t;
using std::forward;
using std::is_same;
using std::tuple_element_t;

template <typename T, typename... ARGS_T>
struct is_one_of : public std::false_type {};

template <typename T, typename U, typename... REST_T>
struct is_one_of<T, U, REST_T...>
    : public std::conditional_t<std::is_same<T, U>::value, std::true_type,
                                is_one_of<T, REST_T...>> {};

template <typename T>
struct is_tuple : public std::false_type {};

template <typename... ARGS_T>
struct is_tuple<std::tuple<ARGS_T...>> : public std::true_type {};

// TODO: this isn't really a great place for this...
template <class T>
struct blank_type_ {
  typedef void type;
};

template <typename T, typename Enable = void>
struct is_collection : public std::false_type {};

template <typename T>
struct is_collection<T, typename blank_type_<typename T::value_type>::type>
    : public std::true_type {};

template <typename T, typename ENABLE = void>
struct reset_if_collection {
  static void exec(T&) {}
};

template <typename T>
struct reset_if_collection<T, enable_if_t<is_collection<T>::value>> {
  static void exec(T& c) { c.clear(); }
};

}  // namespace ABULAFIA_NAMESPACE
#endif
